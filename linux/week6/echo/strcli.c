/*
 * Read a stream socket one line at a time, and write each line back
 * to the sender.
 *
 * Return when the connection is terminated.
 */
#include "myhdr.h"
#include "ntools.h"

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	ssize_t 	n;

	while( fgets(sendline, MAXLINE, fp) != NULL) {
		writen(sockfd, sendline, strlen(sendline));
		if (readline(sockfd, recvline, MAXLINE)==0){
			perror("str_cli: server terminated");
		}
		fputs(recvline, stdout);
	}
}
