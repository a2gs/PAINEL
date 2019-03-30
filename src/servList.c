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

#include "log.h"


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
	log_t log;
	pid_t p = (pid_t)0;

	if(argc != 5){
		fprintf(stderr, "[%s %d] Usage:\n%s <PORT> <FILE> <LOG_FULL_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "Where WWW, XXX, YYY and ZZZ are a combination (surrounded by \"'\" and separated by \"|\") of: REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "\tREDALERT = Red alert\n");
		fprintf(stderr, "\tDBALERT = Database alert\n");
		fprintf(stderr, "\tDBMSG = Database message\n");
		fprintf(stderr, "\tOPALERT = Operation alert\n");
		fprintf(stderr, "\tOPMSG = Operation message\n");
		fprintf(stderr, "\tMSG = Just a message\n");
		fprintf(stderr, "\tDEV = Developer (DEBUG) message\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(logCreate(&log, argv[3], argv[4]) == LOG_NOK){
		fprintf(stderr, "[%s %d] Erro criando log! [%s]\n",time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(-2);
	}

	p = daemonizeWithoutLock(&log);
	if(p == (pid_t)NOK){
		logWrite(&log, LOGMUSTLOGIT, "Cannt daemonize server list!\n");
		logClose(&log);
		return(-3);
	}

	fileName = argv[2];

	logWrite(&log, LOGMUSTLOGIT, "Server List Up! Port: [%s] File: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", argv[1], fileName, p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1){
		logWrite(&log, LOGOPALERT, "Erro bind: [%s]\n", strerror(errno));
		logClose(&log);
		return(-4);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		logWrite(&log, LOGOPALERT, "Erro bind: [%s]\n", strerror(errno));
		logClose(&log);
		return(-5);
	}

	if(listen(listenfd, 250) != 0){
		logWrite(&log, LOGOPALERT, "Erro listen: [%s]\n", strerror(errno));
		logClose(&log);
		return(-6);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			logWrite(&log, LOGOPALERT, "Erro accept: [%s]\n", strerror(errno));
			logClose(&log);
			return(-7);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		logWrite(&log, LOGOPMSG, "Connection from [%s], port [%d] at [%s]\n", clientFrom, portFrom, time_DDMMYYhhmmss());

		f = fopen(fileName, "r");
		if(f == NULL){
			logWrite(&log, LOGOPALERT, "Erro abrindo arquivo [%s] para conexao [%s:%d] as [%s]\n", fileName, clientFrom, portFrom, time_DDMMYYhhmmss());
			logClose(&log);
			return(-8);
		}

		for(i = 0; i <= 10; i++){
			if(i == 10){
				logWrite(&log, LOGOPALERT, "Nao foi liberado o LOCK para o arquivo [%s] em 10 tentativas!\n", fileName);
				logClose(&log);
				return(-9);
			}

			retLock = html_testHtmlLock(f);

			if(retLock == HTML_FILE_LOCKED){
				logWrite(&log, LOGOPMSG, "Arquivo [%s] locked! Aguardando 3 segundos de 10 tentativas: %d/10\n", fileName, i+1);
				sleep(3);
			}else if(retLock == HTML_FILE_UNLOCKED){
				break;
			}else{
				logWrite(&log, LOGOPALERT, "Erro em testar LOCK no aquivo html [%s]!\n", fileName);
				logClose(&log);
				return(-10);
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

	logClose(&log);
	return(0);
}
