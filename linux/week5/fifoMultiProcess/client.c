#include "myhdr.h"
#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];
static char serverFifo[SERVER_FIFO_NAME_LEN];

static void /* Invoked on exit to delete client FIFO */
removeFifo(void) {
    unlink(clientFifo);
    unlink(serverFifo);
}

static void
disconnect(struct request req, int serverFd) {
    int clientFd;
    struct response resp;
    
    req.seqLen = EOSEND; 
    //send final signal 
    if (write(serverFd, &req, sizeof(struct request)) !=
                 sizeof(struct request)) {
            perror("Can't write to server");
    }

    clientFd = open(clientFifo, O_RDONLY);

    if (clientFd == -1) {
        fprintf(stderr,"open %s", clientFifo);
        exit(EXIT_FAILURE);
    }
    if (read(clientFd, &resp, sizeof(struct response))
                        != sizeof(struct response)) {
        fprintf(stderr,"Can't read from server");
        exit(EXIT_FAILURE);
    }
}

void
makefifo() {
    /* Create our FIFO (before sending request, to avoid a race) */
    umask(0); /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(clientFifo, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "mkfifo %s", clientFifo);
        exit(EXIT_FAILURE);
    }
    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(serverFifo, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "mkfifo %s", serverFifo);
        exit(EXIT_FAILURE);
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char *argv[])
{
    int serverConnectFd, serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [seq-len...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    makefifo();
    req.pid = getpid();
    req.seqLen = 1;
        
    /* Construct request message, open server FIFO, and send request */
    serverConnectFd = open(SERVER_CONNECT_FIFO, O_WRONLY);
    if (serverConnectFd == -1){
        fprintf(stderr,"open %s error\n", SERVER_CONNECT_FIFO);
        exit(EXIT_FAILURE);
    }

    //connect req
    if (write(serverConnectFd, &req, sizeof(struct request)) !=
                  sizeof(struct request)) {
        perror("Can't write to server");
        exit(EXIT_FAILURE);
    }

    serverFd = open(serverFifo, O_WRONLY);
    if (serverFd == -1){
        fprintf(stderr,"open %s error\n", serverFifo);
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<10;i++){
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

        //request data
        if (write(serverFd, &req, sizeof(struct request)) !=
                    sizeof(struct request)) {
            perror("Can't write to server");
            exit(EXIT_FAILURE);
        }
    }
    req.pid = getpid();
    disconnect(req, serverFd);   
    close(serverFd);
    sleep(2);
    exit(EXIT_SUCCESS);
}