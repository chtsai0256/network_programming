#include "myhdr.h"
#include "fifo_seqnum.h"
#include <sys/wait.h>
#include <signal.h>

void sigchld_handler(int sig) {
    int saved_errno = errno;
    fprintf(stderr, "child exit\n");
    while(waitpid((pid_t)(-1),0,WNOHANG)>0) {}
    errno = saved_errno;
}

void set_signal_action(void)
{
	struct sigaction act;
	bzero(&act, sizeof(act));
	act.sa_handler = &sigchld_handler;
	sigaction(SIGINT, &act, NULL);
}

void doLoop(struct request req ) {
    int serverFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    char serverFifo[SERVER_FIFO_NAME_LEN];
    struct response resp;
    int seqNum = 0; 


    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE, (long) req.pid);
    if ((serverFd = open(serverFifo, O_RDONLY))<0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }

    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);

    while(1) { 
        /* Open client FIFO (previously created by client) */
        if ((clientFd = open(clientFifo, O_WRONLY))<0) {
            fprintf(stderr, "open %s error\n", clientFifo);
            exit(EXIT_FAILURE);
        }
    
        /* Send response and close FIFO */
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))!= sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);

        if (close(clientFd) < 0)
            fprintf(stderr, "close client fifo error\n");

        seqNum += req.seqLen; /* Update our sequence number */

        /* Read requests and send responses */
        fprintf(stderr, "waiting client connect %d\n", seqNum);
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
        }

        if (req.seqLen == EOSEND) {
            break;
        }
    }
    exit(EXIT_SUCCESS);
}


int
main(int argc, char *argv[])
{
    int serverConnectFd, dummyConnectFd;
    struct request req;

    set_signal_action();

    /* Create well-known FIFO, and open it for reading */
    umask(0); /* So we get the permissions we want */
    if (mkfifo(SERVER_CONNECT_FIFO, 0777) == -1 && errno != EEXIST) {
        perror("server fifo error");
        exit(EXIT_FAILURE);
    }

    if ((serverConnectFd = open(SERVER_CONNECT_FIFO, O_RDONLY))<0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }
    

    /* Open an extra write descriptor, so that we never see EOF */
    if ((dummyConnectFd = open(SERVER_CONNECT_FIFO, O_WRONLY))<0) {
            perror("open server fifo error");
            exit(EXIT_FAILURE);
    }
    //waiting connection req.
    while(1) {
        if (read(serverConnectFd, &req, sizeof(struct request)) == sizeof(struct request)) {
            switch(fork()){
                case -1:
                    perror("fork() error\n");
                    exit(EXIT_FAILURE);
                case  0:
                    close(serverConnectFd);
                    close(dummyConnectFd);
                    doLoop(req);
            default:
                break;
            }
        }
    }
}
