#include "../include/defs.h"

int tcpListen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int				listenfd, n;
	const int		on = 1;
	struct addrinfo	hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;        /* let getaddrinfo() node type be suitable for bind() */
	hints.ai_family = AF_UNSPEC;        /* for any protocol */
	hints.ai_socktype = SOCK_STREAM;    /* for tcp */

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_listen error for %s, %s: %s",
				 host, serv, gai_strerror(n));
	ressave = res;

	for(; res != NULL; res = res->ai_next)
    {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;		/* error, try next one */

        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		    err_sys("setsockopt error");
        /*  
            SOL_SOCKET:     for the socket level.
            SO_REUSEADDR:   the socket level option, for ??
        */
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */
		_close(listenfd);    /* bind error, close and try next one */
	}

	if (res == NULL)	/* errno from final socket() or bind() */
		err_sys("tcp_listen error for %s, %s", host, serv);

	if (listen(listenfd, LISTENQ) < 0)
		err_sys("listen error");

	if (addrlenp)
		*addrlenp = res->ai_addrlen;	/* return size of protocol address */
	freeaddrinfo(ressave);

	return(listenfd);
}

#ifdef	HAVE_SOCKADDR_DL_STRUCT
#include	<net/if_dl.h>
#endif

/* include sock_ntop */
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char		portstr[8];
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}
/* end sock_ntop */

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un *unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s (index %d)",
					 sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char *ptr;

	if ( (ptr = sock_ntop(sa, salen)) == NULL)
		err_sys("sock_ntop error");	/* inet_ntop() sets errno */
	return(ptr);
}