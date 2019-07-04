/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* util_network.c
 * Project's auxiliary network functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 03/07/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "util.h"

#include <log.h>


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static int sockfd = -1; /* Socket */


/* *** FUNCTIONS *********************************************************************** */
/* 0 - Not connect | 1 - Connected */
int isConnect(void)
{
	return((sockfd == -1) ? 0 : 1);
}

int getSocket(void)
{
	return(sockfd);
}

int disconnectSrvPainel(void)
{
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	sockfd = -1;

	return(PAINEL_OK);
}

int connectSrvPainel(char *srvAdd, char *srvPort, int *log)
{
	struct addrinfo hints, *res = NULL, *rp = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	void *pAddr = NULL;
	char strAddr[STRADDR_SZ + 1] = {'\0'};

	if(isConnect() == 1)
		disconnectSrvPainel();

	memset (&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(srvAdd, srvPort, &hints, &res);
	if(errGetAddrInfoCode != 0){
		logWrite(log, LOGOPALERT, "ERRO: getaddrinfo() [%s]. Terminating application with ERRO.\n\n", gai_strerror(errGetAddrInfoCode));
		return(PAINEL_NOK);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			logWrite(log, LOGOPALERT, "ERRO: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		logWrite(log, LOGOPMSG, "Trying connect to [%s/%s:%s].\n", rp->ai_canonname, strAddr, srvPort);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		logWrite(log, LOGOPALERT, "ERRO: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, srvPort, strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		logWrite(log, LOGOPALERT, "ERRO: Unable connect to any address. Terminating application with ERRO.\n\n");
		return(PAINEL_NOK);
	}

	freeaddrinfo(res);

	return(PAINEL_OK);
}
