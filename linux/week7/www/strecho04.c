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
	char    buff1[MAXLINE];

	while( read(sockfd, buff, MAXLINE) > 0) {
		fprintf(stderr, "%s\n",buff);		

		int fd = open("html/index.html", O_RDONLY, 666);
		char *head="HTTP/1.1 200 OK\nContent-length: 60\nContent-Type: text/html\n";
		write(sockfd,head, sizeof(head));
		while((n=read(fd,buff1,MAXLINE))>0) {
			write(sockfd, buff1, n);
		}

	}

	
	exit(0);
}
