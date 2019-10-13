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
	uint32_t msgNetOrderSz = 0, msgHostOderSz = 0;
	unsigned char *iv = (unsigned char *)"0123456789012345"; /* TODO */

	/* memset(netBuff, '\0', MAXLINE + 1); */

	msgHostOderSz = /*srSz =*/ msgSz;
	*sendError = 0;

	msgNetOrderSz = htonl(msgHostOderSz);

	send(sockfd, &msgNetOrderSz, 4, 0); /* Sending the message size in binary. 4 bytes at the beginning */

	if(encrypt_SHA256((unsigned char *)msg, (int) msgSz, (unsigned char *)hashpassphrase, iv, (unsigned char *)netBuff, (int *)&srSz) == PAINEL_NOK){
		return(PAINEL_NOK);
	}

	for(srRet = 0; srRet < (ssize_t)srSz; ){
		srRetAux = send(sockfd, &msg[srRet], srSz - srRet, 0);

		if(srRetAux == -1){
			*sendError = errno;
			return(PAINEL_NOK);
		}

		srRet += srRetAux;
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
	size_t srSz   = 0;
	size_t lessSz = 0;
	ssize_t srRet = 0, srRetAux = 0;
	uint32_t msgNetOrderSz = 0, msgHostOderSz = 0;
	unsigned char *iv = (unsigned char *)"0123456789012345"; /* TODO */

	memset(netBuff, '\0', MAXLINE + 1);
	memset(msg,     '\0', msgSz);

	*recvError = 0;
	*recvSz    = 0;

	recv(sockfd, &msgNetOrderSz, 4, 0);
	msgHostOderSz = ntohl(msgNetOrderSz);

	/* What is smallest? MAXLINE (network buffer), msgSz (user msg buffer) or size sent into protocol? */
	lessSz = ((MAXLINE < msgSz) ? (MAXLINE < msgHostOderSz ? MAXLINE : msgHostOderSz) : (msgSz < msgHostOderSz ? msgSz : msgHostOderSz));

	/* lessSz MUST BE the smallest size inside msg. If there will be more data (msgHostOderSz - lessSz), netBuff will be
	 * copied to msg and the rest os bytes into socket will be burned!
	 */

	srSz = lessSz;

	for(srRet = 0; srRet < (ssize_t)srSz; ){
		srRetAux = recv(sockfd, &netBuff[srRet], MAXLINE, 0);

		if(srRetAux == 0){
			*recvError = 0;
			return(PAINEL_NOK);
		}

		if(srRetAux == -1){
			*recvError = errno;
			return(PAINEL_NOK);
		}

		srRet += srRetAux;
	}
/*
	*recvSz = msgHostOderSz;
*/

	/* memcpy(msg, netBuff, lessSz); */ /* Coping the safe amount of data (the rest will the burned) */

	if(decrypt_SHA256((unsigned char *)netBuff, srRet, (unsigned char *)hashpassphrase, iv, (unsigned char *)msg, (int *)recvSz) == PAINEL_NOK){
		return(PAINEL_NOK);
	}

	srSz = msgHostOderSz - lessSz;
	if(srSz > 0)
		recv(sockfd, netBuff, MAXLINE, 0);

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
