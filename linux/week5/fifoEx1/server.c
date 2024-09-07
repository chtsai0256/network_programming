#include "myhdr.h"
#include "fifo_seqnum.h"
#include <sys/wait.h>
#include <signal.h>


void doLoop(int serverFd) {
    int clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0; 

    while(1) { 
        /* Read requests and send responses */
        fprintf(stderr, "waiting client connect %d\n", seqNum);
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        if ((clientFd = open(clientFifo, O_WRONLY))<0) {
            fprintf(stderr, "open %s error\n", clientFifo);
            continue;
        }
        /* Send response and close FIFO */
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))!= sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);

        if (close(clientFd) < 0)
            fprintf(stderr, "close client fifo error\n");

        seqNum += req.seqLen; /* Update our sequence number */
    }
}


int
main(int argc, char *argv[])
{
    int serverFd, dummyFd;

    /* Create well-known FIFO, and open it for reading */
    umask(0); /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, 0777) == -1 && errno != EEXIST) {
        perror("server fifo error");
        exit(EXIT_FAILURE);
    }

    if ((serverFd = open(SERVER_FIFO, O_RDONLY))<0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }
    

    /* Open an extra write descriptor, so that we never see EOF */
    if ((dummyFd = open(SERVER_FIFO, O_WRONLY))<0) {
            perror("open server fifo error");
            exit(EXIT_FAILURE);
    }
 
    doLoop(serverFd);
}