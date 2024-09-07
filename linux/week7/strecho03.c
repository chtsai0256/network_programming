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
	long arg1, arg2;
	ssize_t 	n;
	char	buff[MAXLINE];

	while( (n = read(sockfd, buff, MAXLINE)) > 0) {
		if (sscanf(buff, "%ld%ld", &arg1, &arg2)==2)
			snprintf(buff, sizeof(buff), "%ld\n", arg1+arg2);
		else	
			snprintf(buff, sizeof(buff), "input error\n");

		n = strlen(buff);
		writen(sockfd, buff, n);
	}
}
