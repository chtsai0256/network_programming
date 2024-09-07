#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include<sys/wait.h>


#define serverPort 5000

typedef struct server_socket
{
    pid_t pid;
    struct server_socket *next;
    struct server_socket *prev;
} serverSock_t;

serverSock_t *sSockList = NULL;

void sigchld_handler(int sig) {
    int childpid=0;
    serverSock_t *ptr=sSockList;
    int saved_errno = errno;
    fprintf(stderr, "child exit\n");
    while((childpid=waitpid((pid_t)(-1),0,WNOHANG))>0) {}
    errno = saved_errno;
    while(ptr!=NULL) {
        if (ptr->pid==childpid) {
            if (ptr==sSockList) {
                sSockList = NULL;
            } else {
                ptr->prev->next = ptr->next;
                ptr->next->prev = ptr->prev;
            }
            free(ptr);
            fprintf(stderr, "%d child stop\n", childpid);
            exit(0);
        }
        ptr = ptr->next;
    }
}

void set_signal_action(void)
{
	struct sigaction act;
	bzero(&act, sizeof(act));
	act.sa_handler = &sigchld_handler;
	sigaction(SIGCHLD, &act, NULL);
}


int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(serverPort); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    set_signal_action();
    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        //close(connfd);
        pid_t p = fork();
        switch(p) {
            case -1:
                perror("fork");
                break;
            case 0:
                dup2(connfd, STDOUT_FILENO);
                dup2(connfd, STDIN_FILENO);
                close(connfd);
                setbuf(stdout, NULL);
                startShell();
                exit(0);
                break;
            default:
                serverSock_t *newNode = (serverSock_t *) malloc(sizeof(serverSock_t));
                newNode->pid = p;
                bzero(newNode, sizeof(serverSock_t));
                if (sSockList==NULL) {
                    sSockList = newNode;
                } else {
                    newNode->next = sSockList;
                    sSockList->prev = newNode;
                    sSockList = newNode; 
                }
        }
     }
}