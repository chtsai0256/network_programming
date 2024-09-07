#include "../include/defs.h"

ssize_t readn(int fd, void *vptr, size_t n)
{   /* Read "n" bytes from a descriptor. */
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {

			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}

	return(n - nleft);		/* return >= 0 */
}

ssize_t writen(int fd, const void *vptr, size_t n) 
{   /* Write "n" bytes to a descriptor. */
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

ssize_t _read(int fd, void *ptr, size_t nbytes)
{
	ssize_t	n;
	if ( (n = readn(fd, ptr, nbytes)) < 0)
		err_sys("readn error");
	return(n);
}

void _write(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

void _close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

