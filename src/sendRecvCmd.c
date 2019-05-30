/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* sendRecvCmd.c
 * Send and receive buffers readed from a file.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 30/05/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "util.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int main(int argc, char *argv[])
 *
 * INPUT:
 *  argv[1] - Server IP address
 *  argv[2] - Server port
 * OUTPUT (to SO):
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	int sockfd = 0;
	int errGetAddrInfoCode = 0, errConnect = 0;
	char strAddr[STRADDR_SZ + 1] = {'\0'};
	void *pAddr = NULL;
	FILE *f = NULL;
	char line[MAXLINE] = {'\0'};
	char *c = NULL;
	struct addrinfo hints, *res = NULL, *rp = NULL;
	int srError = 0;
	size_t recvSz = 0;

	if(argc != 4){
		fprintf(stderr, "[%s %d] Usage:\n%s <IP_ADDRESS> <PORT> <FILE_CMDs>\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	getLogSystem_Util(NULL);

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	fprintf(stderr, "StartUp Client [%s]! Server: [%s] Port: [%s] Cmd file: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), argv[1], argv[2], argv[3], getPAINELEnvHomeVar());

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(argv[1], argv[2], &hints, &res);
	if(errGetAddrInfoCode != 0){
		fprintf(stderr, "ERRO: getaddrinfo() [%s]. Terminating application with ERRO.\n", gai_strerror(errGetAddrInfoCode));
		return(-3);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			fprintf(stderr, "ERRO: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		fprintf(stderr, "Trying connect to [%s/%s:%s].\n", rp->ai_canonname, strAddr, argv[2]);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		fprintf(stderr, "ERRO: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, argv[2], strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		fprintf(stderr, "ERRO: Unable connect to any address. Terminating application with ERRO.\n");
		return(-4);
	}

	freeaddrinfo(res);

	f = fopen(argv[1], "r");
	if(f == NULL){
		printf("Unable to open file: [%s]\n", strerror(errno));
		return(-1);
	}

	for(; fgets(line, MAXLINE, f) != NULL; ){
		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		if(sendToNet(sockfd, line, strlen(line), &srError) == PAINEL_NOK){
			fprintf(stderr, "sendToNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}

		if(recvFromNet(sockfd, line, MAXLINE, &recvSz, &srError) == PAINEL_NOK){
			fprintf(stderr, "recvFromNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}

	}

	fclose(f);

	fprintf(stderr, "Terminating application with sucessfully!\n");

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return(0);
}
