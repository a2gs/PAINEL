/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* servList.c
 * Server to send HTML file to browsers.
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
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define HTML_START_PROT1		("HTTP/1.1 200 OK\r\n")
#define HTML_START_PROT2		("Content-Type: text/html; charset=UTF-8\r\n\r\n")


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int main(int argc, char **argv)
 *
 * servList starts.
 *
 * INPUT:
 *  argv[1] - Server port to listen
 *  argv[2] - File to send through socket
 * OUTPUT:
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char **argv)
{
	int listenfd = 0, connfd = 0, i = 0, retLock = 0;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char addStr[255 + 1] = {'\0'};
	char msg[MAXLINE] = {'\0'};
	char clientFrom[200] = {'\0'};
	uint16_t portFrom = 0;
	FILE *f = NULL;
	char *fileName = NULL;
	pid_t p = (pid_t)0;

	if(argc != 3){
		fprintf(stderr, "%s <PORT> <FILE>\n", argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	fileName = argv[2];

	p = daemonizeWithoutLock();
	if(p == (pid_t)NOK){
		fprintf(stderr, "Cannt daemonize server!\n");
		return(-2);
	}

	fprintf(stderr, "Server List Up! Port: [%s] File: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", argv[1], fileName, p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		fprintf(stderr, "Erro bind: [%s]\n", strerror(errno));
		return(-3);
	}

	if(listen(listenfd, 250) != 0){
		fprintf(stderr, "Erro listen: [%s]\n", strerror(errno));
		return(-4);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			fprintf(stderr, "Erro accept: [%s]\n", strerror(errno));
			return(-5);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		fprintf(stderr, "Connection from [%s], port [%d] at [%s]\n", clientFrom, portFrom, time_DDMMYYhhmmss());

		f = fopen(fileName, "r");
		if(f == NULL){
			fprintf(stderr, "Erro abrindo arquivo [%s] para conexao [%s:%d] as [%s]\n", fileName, clientFrom, portFrom, time_DDMMYYhhmmss());
			return(-6);
		}

		for(i = 0; i <= 10; i++){
			if(i == 10){
				fprintf(stderr, "Nao foi liberado o LOCK para o arquivo [%s] em 10 tentativas!\n", fileName);
				return(-7);
			}

			retLock = html_testHtmlLock(f);

			if(retLock == HTML_FILE_LOCKED){
				fprintf(stderr, "Arquivo [%s] locked! Aguardando 3 segundos de 10 tentativas: %d/10\n", fileName, i+1);
				sleep(3);
			}else if(retLock == HTML_FILE_UNLOCKED){
				break;
			}else{
				fprintf(stderr, "Erro em testar LOCK no aquivo html [%s]!\n", fileName);
				return(-8);
			}
		}

		send(connfd, HTML_START_PROT1, sizeof(HTML_START_PROT1)-1, /*MSG_DONTWAIT|MSG_MORE*/0);
		send(connfd, HTML_START_PROT2, sizeof(HTML_START_PROT2)-1, /*MSG_DONTWAIT|MSG_MORE*/0);

		while(!feof(f)){
			memset(msg, '\0', MAXLINE);

			if(fgets(msg, MAXLINE, f) == NULL) break;

			send(connfd, msg, strlen(msg), /*MSG_DONTWAIT|MSG_MORE*/0);
		}

		send(connfd, "\r\n", 2, /*MSG_DONTWAIT*/0);

		shutdown(connfd, SHUT_RDWR);
		close(connfd);
	}

	return(0);
}
