#include "myhdr.h"
#include "ntools.h"
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv) {
    int i, sockfd[5];
    struct sockaddr_in servaddr;
      
    for (i=0;i<5;i++) {
        if ( (sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket error");
            exit(0);
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(SERV_PORT);
        inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

        if (connect(sockfd[i], (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            perror("connect error");
            exit(0);
        }
    }
    str_cli(stdin, sockfd[0]);
    exit(0);
}
        