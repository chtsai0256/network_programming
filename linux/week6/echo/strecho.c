/*
 * Read a stream socket one line at a time, and write each line back
 * to the sender.
 *
 * Return when the connection is terminated.
 */
#include "myhdr.h"
#include "ntools.h"

void str_echo(int sockfd)
{
	ssize_t 	n;
	char	buff[MAXLINE];

	//again:
	while( (n = read(sockfd, buff, MAXLINE)) > 0) {
		//buff[n]='\0';
		//fprintf(stderr, "%s, size=%ld\n", buff,n);
		writen(sockfd, buff, n);
	}
	
	//if (n<0 && errno == EINTR)
	//	goto again;
	//else if (n<0)
	//	perror("read error");
	exit(0);
}
