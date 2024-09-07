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
	args_t  args;
	result_t result;

	while( read(sockfd, &args, sizeof(args)) > 0) {
		result.sum = args.arg1+args.arg2;
		writen(sockfd, &result, sizeof(result));
	}
}
