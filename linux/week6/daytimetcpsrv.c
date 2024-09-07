#include "myhdr.h"
#include "ntools.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAXLINE];
    time_t ticks;
      
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13); /* daytime server */
    
    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(0);
    }

    if (listen(listenfd, 5) != 0 ) {
        perror("listen error");
        exit(0);
    }

    while(1) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
        fprintf(stderr, "connection from %s, port %d\n", 
                inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                ntohs(cliaddr.sin_port) );
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        if (write(connfd, buff, strlen(buff))!=strlen(buff)) {
            perror("write error");
        };
        close(connfd);
    }
}
        