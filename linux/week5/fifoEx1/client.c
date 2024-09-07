#include "myhdr.h"
#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void /* Invoked on exit to delete client FIFO */
removeFifo(void) {
    unlink(clientFifo);
}

int
main(int argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Create our FIFO (before sending request, to avoid a race) */
    umask(0); /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST) {
        fprintf(stderr, "mkfifo %s", clientFifo);
        exit(EXIT_FAILURE);
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    req.pid = getpid();
    req.seqLen = 1;
        
    /* Construct request message, open server FIFO, and send request */
    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1){
        fprintf(stderr,"open %s error\n", SERVER_FIFO);
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<10;i++){
        if (write(serverFd, &req, sizeof(struct request)) !=
                    sizeof(struct request)) {
            perror("Can't write to server");
            exit(EXIT_FAILURE);
        }
        /* Open our FIFO, read and display response */
        clientFd = open(clientFifo, O_RDONLY);
        if (clientFd == -1) {
            fprintf(stderr,"open %s", clientFifo);
            exit(EXIT_FAILURE);
        }
    
        if (read(clientFd, &resp, sizeof(struct response))
                        != sizeof(struct response)) {
            perror("Can't read response from server");
            exit(EXIT_FAILURE);
        }
        printf("response %d\n", resp.seqNum);
        sleep(1);
        close(clientFd);
    }
    close(serverFd);
    exit(EXIT_SUCCESS);
}