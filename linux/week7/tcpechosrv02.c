#include "myhdr.h"
#include "ntools.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

void sigchld_handler(int sig) {
    int saved_errno = errno;
    while(waitpid((pid_t)(-1),0,WNOHANG)>0) {}
    errno = saved_errno;
}

void set_signal_action(void)
{
	struct sigaction act;
	bzero(&act, sizeof(act));
	act.sa_handler = &sigchld_handler;
	sigaction(SIGCHLD, &act, NULL);
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
      
    set_signal_action();
    
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(0);
    }

    if (listen(listenfd, 5) != 0 ) {
        perror("listen error");
        exit(0);
    }

    while(1) {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen))<0 ) {
            if (errno == EINTR)
                continue;
            else {
                perror("accept error");
                exit(0);
            }
        }
        if ( (childpid = fork()) ==0 ) { //child process
            close(listenfd);     // close listening socket in child process
            str_echo(connfd);    // process the request
            exit(0);
        };
        close(connfd); //close connected socket in parent process
    }
}
        