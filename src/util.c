/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* util.c
 * Project's auxiliary functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "util.h"

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOGPATH_SZ		(350)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */
static char netBuff[MAXLINE + 1] = {0};


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
char * getPAINELEnvHomeVar(void)
{
	return(getenv(PAINEL_HOME_ENV));
}

inline int changeCharByChar(char *buffer, int from, int to)
{
	char *p = NULL;

	p = strchr(buffer, from);

	if(p == NULL) return(PAINEL_NOK);

	*p = to;

	return(PAINEL_OK);
}

inline size_t cutter(char **buffer, int c, char *out, size_t outSz)
{
	char *c1 = NULL, *c2 = NULL;
	size_t tam = 0, less = 0;

	for(c1 = c2 = *buffer; (*c2 != '\0') && (*c2 != c); c2++);

	tam = c2 - c1;
	less = ((outSz < tam) ? outSz : tam);
	strncpy(out, c1, less);

	out[less] = '\0';

	if(*c2 == '\0') *buffer = c2;
	else            *buffer = c2+1;

	return(tam);
}

inline char * time_DDMMYYhhmmss(void)
{
	static char segmil[DATA_LEN + 1] = {'\0'};
	time_t rawtime;
	struct tm *tmTime = NULL;

	rawtime = time(0);
	tmTime = localtime(&rawtime);
	strftime((char *)segmil, DATA_LEN, "%d/%m/%y %H:%M:%S", tmTime);

	segmil[DATA_LEN - 1]='\0';

	return(segmil);
}

/* ---[DUAL HTML]------------------------------------------------------------------------------------------ */
int html_fopen(htmlFiles_t *htmls, char *htmlStatic, char *htmlRefresh)
{
	/* Opening static */
	htmls->htmlStatic = fopen(htmlStatic, "w");
	if(htmls->htmlStatic == NULL){
		fprintf(stderr, "Erro criando HTML STATIC [%s]. Motivo [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	htmls->fdStatic = fileno(htmls->htmlStatic);
	if(htmls->fdStatic == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	/* Opening refresh */
	htmls->htmlRefresh = fopen(htmlRefresh, "w");
	if(htmls->htmlRefresh == NULL){
		fprintf(stderr, "Erro criando HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	htmls->fdRefresh = fileno(htmls->htmlRefresh);
	if(htmls->fdRefresh == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	/* Locking */
	if(lockf(htmls->fdStatic, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	if(lockf(htmls->fdRefresh, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	return(PAINEL_OK);
}

int html_writeDual(htmlFiles_t *htmls, int files, char *msg)
{
	if(files == 0){
		fprintf(htmls->htmlStatic, "%s", msg);
		fprintf(htmls->htmlRefresh, "%s", msg);
	}
	else if(files == 1) fprintf(htmls->htmlStatic,  "%s", msg);
	else if(files == 2) fprintf(htmls->htmlRefresh, "%s", msg);

	return(PAINEL_OK);
}

int html_fflush(htmlFiles_t *htmls)
{
	fflush(htmls->htmlStatic );
	fflush(htmls->htmlRefresh);
	return(PAINEL_OK);
}

int html_fclose(htmlFiles_t *htmls)
{
	/* Unlocking */
	if(lockf(htmls->fdStatic, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(PAINEL_NOK);
	}

	if(lockf(htmls->fdRefresh, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(PAINEL_NOK);
	}

	/* Closing */
	fclose(htmls->htmlStatic);
	fclose(htmls->htmlRefresh);

	memset(htmls, 0, sizeof(htmlFiles_t));

	return(PAINEL_OK);
}

int html_testHtmlLock(FILE *file)
{
	int fd = 0;

	fd = fileno(file);
	if(fd == -1){
		fprintf(stderr, "Erro fileno: %s\n", strerror(errno));
		return(HTML_FILE_LOCKERR);
	}

	if(lockf(fd, F_TEST, 0) < 0)
		return(HTML_FILE_UNLOCKED);

	return(HTML_FILE_UNLOCKED);
}

/* ---[DAEMONIZE]------------------------------------------------------------------------------------------ */
/* void signal_handlerWithoutLock(int sig)
 *
 * Daemon signal handler (this function has no 'process lock file' to unlock).
 *
 * INPUT:
 *  sig - signal received
 * OUTPUT:
 *  none
 */
static log_t *logUtil = NULL;

void signal_handlerWithoutLock(int sig)
{
	logWrite(logUtil, LOGMUSTLOGIT, "Got signal [%d] at [%s]!\n", sig, time_DDMMYYhhmmss());

	switch(sig){
		case SIGHUP:
			break;
		case SIGTERM:
			break;
	}

	if(logUtil != NULL)
		logClose(logUtil);

	if(logUtil->fd != -1)
		logClose(logUtil);

	exit(0);
	return;
}

pid_t daemonizeWithoutLock(log_t *log)
{
	pid_t i = 0, father = 0;

	logUtil = NULL;

	father = getppid();
	if(father == 1)
		return(PAINEL_NOK);

	i = fork();

	father = i;

	if(i == -1){
		fprintf(stderr, "Erro fork chield process! [%s]\n", strerror(errno));
		return((pid_t)PAINEL_NOK);
	}

	if(i > 0)
		exit(0); /* Pai termina: 0 - Ok */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	chdir("./");

	logUtil = log;

	/* Configurando sinais */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP, signal_handlerWithoutLock);
	signal(SIGTERM, signal_handlerWithoutLock);
	signal(SIGPIPE, signal_handlerWithoutLock);

	return(getpid());
}

int sendToNet(int sockfd, char *msg, size_t msgSz, int *sendError) /* TODO: receber size_t * indicando o quanto foi enviado */
{
	ssize_t srRet = 0, srRetAux = 0;
	size_t srSz = 0;
	uint32_t msgNetOrderSz = 0, msgHostOderSz = 0;

	memset(netBuff, '\0', MAXLINE + 1);

	msgHostOderSz = srSz = msgSz; /* TODO: muita variavel ... acho q da pra suprir algumas */

	msgNetOrderSz = htonl(msgHostOderSz);
	send(sockfd, &msgNetOrderSz, 4, 0); /* Sending the message size in binary. 4 bytes at the beginning */

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
int recvFromNet(int sockfd, char *msg, size_t msgSz, size_t *recvSz, int *recvError)
{
	ssize_t srRet = 0, srRetAux = 0;
	size_t srSz = 0;
	size_t lessSz = 0;
	uint32_t msgNetOrderSz = 0, msgHostOderSz = 0;

	memset(netBuff, '\0', MAXLINE + 1);
	memset(msg, '\0', msgSz);

	*recvError = 0;
	*recvSz = 0;

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

	*recvSz = msgHostOderSz;

	memcpy(msg, netBuff, lessSz); /* Coping the safe amount of data (the rest will the burned) */

	srSz = msgHostOderSz - lessSz;
	if(srSz > 0)
		recv(sockfd, netBuff, MAXLINE, 0);

	return(PAINEL_OK);
}

int pingServer(char *ip, char *port)
{
	int sockfd = 0;
	int srError = 0;
	size_t msgSRSz = 0;
	char *c = NULL;
	void *pAddr = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	char strAddr[STRADDR_SZ + 1] = {'\0'};
	struct addrinfo hints, *res = NULL, *rp = NULL;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(ip, port, &hints, &res);
	if(errGetAddrInfoCode != 0){
		logWrite(logUtil, LOGOPALERT, "ERRO PING: getaddrinfo() [%s]. Terminating application with ERRO.\n\n", gai_strerror(errGetAddrInfoCode));
		return(PAINEL_NOK);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			logWrite(logUtil, LOGOPALERT, "ERRO PING: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		logWrite(logUtil, LOGOPMSG, "Trying PING (connect) to [%s/%s:%s].\n", rp->ai_canonname, strAddr, port);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		logWrite(logUtil, LOGOPALERT, "ERRO PING: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, port, strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		logWrite(logUtil, LOGOPALERT, "ERRO PING: Unable connect to any address. Terminating application with ERRO.\n\n");
		return(PAINEL_NOK);
	}

	freeaddrinfo(res);

	msgSRSz = 0; srError = 0;
	msgSRSz = snprintf(netBuff, MAXLINE, "%d|PING", PROT_COD_PING);
	memset(netBuff, '\0', MAXLINE + 1);
	if(sendToNet(sockfd, netBuff, msgSRSz, &srError) == PAINEL_NOK){
		logWrite(logUtil, LOGOPALERT, "ERRO PING: Unable to SEND ping: [%s].\n", strerror(srError));
		return(PAINEL_NOK);
	}

	msgSRSz = 0; srError = 0;
	memset(netBuff, '\0', MAXLINE + 1);
	if(recvFromNet(sockfd, netBuff, MAXLINE, &msgSRSz, &srError) == PAINEL_NOK){
		logWrite(logUtil, LOGOPALERT, "ERRO PING: Unable to RECV ping: [%s].\n", strerror(srError));
		return(PAINEL_NOK);
	}

	c = strchr(netBuff, '|');
	if(c == NULL) c = netBuff;
	else c++;

	logWrite(logUtil, LOGOPMSG, "Ping response from server: [%s].\n", c);

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return(PAINEL_OK);
}
