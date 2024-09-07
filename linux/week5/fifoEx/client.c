#include "fifo_seqnum.h"
#include "myhdr.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void 
removeFifo(void) {
    unlink(clientFifo);
}

static void sendPacket(int OUT, request_t *req) {
    int count = write(OUT, req, sizeof(request_t));
    if (count != sizeof(request_t)) {
        perror("Can't write to server");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "send from client id %d seq %d to server\n", req->pid, req->seqLen);
}

static void rxPacket(int IN, response_t *resp) {
    int count = read(IN, resp, sizeof(response_t));
    if (count!= sizeof(response_t)){
        perror("Can't read response from server");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "read resp %d from server \n", resp->seqNum);
}

static void
doTask(int OUT, request_t req) {
    int IN;
    response_t resp;
    while(1) {
        sleep(1);
        sendPacket(OUT, &req);
        /* Open our FIFO, read and display response */
        IN = open(clientFifo, O_RDONLY);
        if (IN<0) {
            perror("open client fifo error");
            exit(EXIT_FAILURE);
        }
        sleep(1);
        fprintf(stderr, "delay 2 seconds \n");        
        rxPacket(IN, &resp);
        req.seqLen = resp.seqNum;
        close(IN);
    }
}

int
main(int argc, char *argv[]) {
    request_t req;
    int OUT, IN;
    
    umask(0); /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(clientFifo, 0777) == -1 && errno != EEXIST) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit error");
        exit(EXIT_FAILURE);
    }
 
    /* Construct request message, open server FIFO, and send request */
    OUT = open(SERVER_FIFO, O_WRONLY);
    if (OUT < 0) {
        perror("open server fifo error");
        exit(EXIT_FAILURE);
    }
 
    req.pid = getpid();
    req.seqLen = 1;
    doTask(OUT, req);
    close(OUT);
    exit(EXIT_SUCCESS);
}