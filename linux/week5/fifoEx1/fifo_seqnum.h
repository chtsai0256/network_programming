#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SERVER_FIFO "/tmp/seqnum_sv"
#define SERVER_FIFO_TEMPLATE "/tmp/seqnum_sv.%ld"
#define SERVER_FIFO_NAME_LEN (sizeof(SERVER_FIFO_TEMPLATE) + 20)
#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
 /* Space required for client FIFO pathname
 (+20 as a generous allowance for the PID) */


/* Request packet structure(client --> server) */
struct request { 
 pid_t pid; /* PID of client */
 unsigned int seqLen; /* Length of desired sequence */
};

/* Response (server --> client) */
struct response { 
 unsigned int seqNum; /* Start of sequence */
};