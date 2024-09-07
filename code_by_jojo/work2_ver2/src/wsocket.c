#include "../include/defs.h"

int _accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int	n;
again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			err_sys("accept error");
	}

	return(n);
}
