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
static log_t *log = NULL;
static int sockfd = -1; /* Socket */
static char netBuff[MAXLINE + 1] = {0};


/* *** FUNCTIONS *********************************************************************** */
void getLogSystem_UtilNetwork(log_t *lg)
{
	log = lg;
	return;
}

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

int connectSrvPainel(char *srvAdd, char *srvPort)
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

int sendToNet(int sockfd, char *msg, size_t msgSz, int *sendError, char *hashpassphrase) /* TODO: receber size_t * indicando o quanto foi enviado */
{
	ssize_t srRet = 0, srRetAux = 0;
	size_t srSz = 0;
	uint32_t msgNetOrderSz = 0;
	unsigned char *iv = (unsigned char *)"0123456789012345"; /* TODO */

	*sendError = 0;

#ifdef PAINEL_NETWORK_DUMP
	{
		unsigned char *dumpMsg = NULL;

		dumpHexBuff(&msg, msgSz, &dumpMsg);
		logWrite(log, LOGDEV, "Sending unencrypted msg (%ld bytes):\n%s\n", msgSz, dumpMsg);
		free(dumpMsg);
	}
#endif

	/* 1. Encrypt */
	if(encrypt_SHA256((unsigned char *)msg, (int) msgSz, (unsigned char *)hashpassphrase, iv, (unsigned char *)netBuff, (int *)&srSz) == PAINEL_NOK){
		return(PAINEL_NOK);
	}

	/* 2. Send encrypted msg size */
	msgNetOrderSz = htonl(srSz);

	srRet = send(sockfd, &msgNetOrderSz, 4, 0); /* Sending the message size in binary. 4 bytes at the beginning */
	if(srRet == -1){
		*sendError = errno;
		return(PAINEL_NOK);
	}
	
#ifdef PAINEL_NETWORK_DUMP
	{
		unsigned char *dumpMsg = NULL;

		dumpHexBuff(&msgNetOrderSz, (size_t)4, &dumpMsg);
		logWrite(log, LOGDEV, "Sending msg size (%ld bytes - network byte order):\n%s\n", srSz, dumpMsg);
		free(dumpMsg);

		dumpHexBuff(&netBuff, (size_t)srSz, &dumpMsg);
		logWrite(log, LOGDEV, "Sending encrypted msg (%ld bytes):\n%s\n", srSz, dumpMsg);
		free(dumpMsg);
	}
#endif

	/* 3. Send encrypted msg size */
	for(srRet = 0, srRetAux = 0; srRet < (ssize_t)srSz; srRet += srRetAux){
		srRetAux = send(sockfd, &netBuff[srRet], srSz - srRet, 0);

		if(srRetAux == -1){
			*sendError = errno;
			return(PAINEL_NOK);
		}
	}

	return(PAINEL_OK);
}

/*
retornando (PAINEL_NOK && recvError == 0): recv erro: Connection close unexpected!
retornando (PAINEL_NOK && recvError != 0): recv erro. recvError mesmo valor de errno
retornando (PAINEL_OK): 
*/
int recvFromNet(int sockfd, char *msg, size_t msgSz, size_t *recvSz, int *recvError, char *hashpassphrase)
{
	unsigned char *iv = (unsigned char *)"0123456789012345"; /* TODO */
	uint32_t msgNetOrderSz = 0, msgNetSz = 0;
	ssize_t retRecv = 0, totRecv = 0;

	memset(netBuff, '\0', MAXLINE + 1);
	memset(msg,     '\0', msgSz);
	*recvError = 0;

	/* 1. Receive encrypted msg size */
	retRecv = recv(sockfd, &msgNetOrderSz, 4, 0);
	if(retRecv == -1){
		*recvError = errno;
		return(PAINEL_NOK);
	}

	msgNetSz = ntohl(msgNetOrderSz);

	/* 2. Read encrypted msg */
	for(retRecv = 0, totRecv = 0; totRecv < (ssize_t)msgNetSz; totRecv += retRecv){

		retRecv = recv(sockfd, &netBuff[totRecv], MAXLINE, 0);

		if(retRecv == 0){
			*recvError = 0;
			return(PAINEL_NOK);
		}

		if(retRecv == -1){
			*recvError = errno;
			return(PAINEL_NOK);
		}
	}

#ifdef PAINEL_NETWORK_DUMP
	{
		unsigned char *dumpMsg = NULL;

		dumpHexBuff(&msgNetOrderSz, (size_t)4, &dumpMsg);
		logWrite(log, LOGDEV, "Received msg size (%ld bytes - network byte order): ", msgNetSz);
		logWrite(log, LOGDEV, "%s\n", dumpMsg);
		free(dumpMsg);

		dumpHexBuff(&netBuff, (size_t)msgNetSz, &dumpMsg);
		logWrite(log, LOGDEV, "Received encrypted msg (%ld bytes):\n", msgNetSz);
		logWrite(log, LOGDEV, "%s\n", dumpMsg);
		free(dumpMsg);
	}
#endif

	/* 3. Decrypt */
	if(decrypt_SHA256((unsigned char *)netBuff, msgNetSz, (unsigned char *)hashpassphrase, iv, (unsigned char *)msg, (int *)recvSz) == PAINEL_NOK){
		return(PAINEL_NOK);
	}

#ifdef PAINEL_NETWORK_DUMP
	{
		unsigned char *dumpMsg = NULL;

		dumpHexBuff(&msg, msgSz, &dumpMsg);
		logWrite(log, LOGDEV, "Received unencrypted msg (%ld bytes):\n", recvSz);
		logWrite(log, LOGDEV, "%s\n", dumpMsg);
		free(dumpMsg);
	}
#endif

	return(PAINEL_OK);
}

int pingServer(char *ip, char *port, netpass_t *netcrypt)
{
	int sockfd = 0;
	int srError = 0;
	size_t msgSRSz = 0;
	void *pAddr = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	char strAddr[STRADDR_SZ + 1] = {'\0'};
	struct addrinfo hints, *res = NULL, *rp = NULL;
	char pingPongMsg[PINGPONG_MBL_SZ + 1] = {'\0'};

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(ip, port, &hints, &res);
	if(errGetAddrInfoCode != 0){
		logWrite(log, LOGOPALERT, "ERRO PING: getaddrinfo() [%s].\n", gai_strerror(errGetAddrInfoCode));
		return(PAINEL_NOK);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			logWrite(log, LOGOPALERT, "ERRO PING: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		logWrite(log, LOGOPMSG, "Trying PING (connect) to [%s/%s:%s].\n", rp->ai_canonname, strAddr, port);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		logWrite(log, LOGOPALERT, "ERRO PING: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, port, strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		logWrite(log, LOGOPALERT, "ERRO PING: Unable connect to any address.\n");
		return(PAINEL_NOK);
	}

	freeaddrinfo(res);

	msgSRSz = 0; srError = 0;
	memset(pingPongMsg, '\0', PINGPONG_MBL_SZ + 1);
	msgSRSz = snprintf(pingPongMsg, PINGPONG_MBL_SZ, "%d|PING", PROT_COD_PING);

	logWrite(log, LOGOPMSG, "Ping sending to server: [%s].\n", pingPongMsg);

	if(sendToNet(sockfd, pingPongMsg, msgSRSz, &srError, NULL) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "ERRO PING: Unable to SEND ping: [%s].\n", strerror(srError));
		shutdown(sockfd, SHUT_RDWR);
		close(sockfd);
		return(PAINEL_NOK);
	}

	msgSRSz = 0; srError = 0;
	memset(pingPongMsg, '\0', PINGPONG_MBL_SZ + 1);

	if(recvFromNet(sockfd, pingPongMsg, PINGPONG_MBL_SZ, &msgSRSz, &srError, NULL) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "ERRO PING: Unable to RECV ping: [%s].\n", strerror(srError));
		shutdown(sockfd, SHUT_RDWR);
		close(sockfd);
		return(PAINEL_NOK);
	}

	logWrite(log, LOGOPMSG, "Ping response from server: [%s].\n", pingPongMsg);

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return(PAINEL_OK);
}
