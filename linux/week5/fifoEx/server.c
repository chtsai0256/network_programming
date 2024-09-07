#include <signal.h>
#include "fifo_seqnum.h"
#include "myhdr.h"



void doLoop(request_t req) {
    int clientFd, serverFd, dummyFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    char serverFifo[SERVER_FIFO_NAME_LEN];
    request_t req;
    response_t resp;
    unsigned int seqNum = 1; 

    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE, (long) req.pid);
    umask(0); /* So we get the permissions we want */
    if (mkfifo(serverFifo, 0777) == -1 && errno != EEXIST) {
        perror("server data fifo error");
        exit(EXIT_FAILURE);
    }
    
    serverFd = open(serverFifo, O_RDONLY);
    if (serverFd<0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }
    /* Open an extra write descriptor, so that we never see EOF */
    if ((dummyFd = open(SERVER_FIFO, O_WRONLY))<0) {
            perror("open server fifo error");
            exit(EXIT_FAILURE);
    }


    while(1) { /* Read requests and send responses */
        //send response to client fifo
        /* Open client FIFO (previously created by client) */
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd<0) {
            fprintf(stderr, "open %s error\n", clientFifo);
            continue;
        }

        /* Send response and close FIFO */
        resp.seqNum = seqNum;

        if (write(clientFd, &resp, sizeof(response_t))!= sizeof(response_t))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);

        if (close(clientFd) < 0)
            fprintf(stderr, "close client fifo error\n");
        seqNum += 1; /* Update our sequence number */

        if (read(serverFd, &req, sizeof(request_t)) != sizeof(request_t)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        }
    }
}


int
main(int argc, char *argv[])
{
    int serverConnectFd, dummyServerFd;
    request_t req;
    /* Create well-known FIFO, and open it for reading */
    umask(0); /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, 0777) == -1 && errno != EEXIST) {
        perror("server fifo error");
        exit(EXIT_FAILURE);
    }
    
    serverConnectFd = open(SERVER_FIFO, O_RDONLY);
    if (serverConnectFd<0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }
    

    /* Open an extra write descriptor, so that we never see EOF */
    if ((dummyServerFd = open(SERVER_FIFO, O_WRONLY))<0) {
            perror("open server fifo error");
            exit(EXIT_FAILURE);
    }
 
    if (read(serverConnectFd, &req, sizeof(request_t)) != sizeof(request_t)) {
        fprintf(stderr, "Error reading request; discarding\n");
    } else {
        switch(fork()){
            case -1:
                perror("for() error\n");
                exit(EXIT_FAILURE);
            case  0:
                close(serverConnectFd);
                close(dummyServerFd);
                doLoop(req);
                break;
            default:
                break;
        }
    }
   doLoop(req);
}