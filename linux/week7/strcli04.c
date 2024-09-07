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
	char sendline[MAXLINE];
	args_t args;
	result_t result;

	while( fgets(sendline, MAXLINE, fp) != NULL) {
		if (sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2)!=2) {
			printf("invalid input: %s", sendline);
			continue;
		}

		writen(sockfd, &args, sizeof(args));

		if (readn(sockfd, &result, sizeof(result))==0){
			perror("str_cli: server terminated");
			exit(0);
		}
		printf("%ld\n", result.sum);
	}
}
