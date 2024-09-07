/*
 * Definitions for TCP and UDP client/server programs.
 */

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include "myhdr.h"

#define	SERV_UDP_PORT	16000
#define	SERV_TCP_PORT	16000
#define	SERV_HOST_ADDR	"134.208.6.49"	/* host addr for server */

char	*pname;
