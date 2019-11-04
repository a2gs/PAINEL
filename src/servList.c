/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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

#include <log.h>
#include <cfgFile.h>


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
	int listenfd = 0, connfd = 0, i = 0, retLock = 0, srvListPort = 0;
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

	unsigned int cfgLineError = 0;
	/* char *cfgServerAddress = NULL; */
	char *cfgServerPort    = NULL;
	char *cfgLogFile       = NULL;
	char *cfgLogLevel      = NULL;
	cfgFile_t srvCfg;

	if(argc != 2){
		fprintf(stderr, "[%s %d] Usage:\n%s <CONFIG_FILE>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "CONFIG_FILE sample variables:\n");
		fprintf(stderr, "\t#PAINEL_BIND_ADDRESS = 123.123.123.123 # (not implemented yet) or DNS\n");
		fprintf(stderr, "\tPAINEL_SERVER_PORT = 9998\n");
		fprintf(stderr, "\tHTML_FILE = servList_Funcao.html\n");
		fprintf(stderr, "\tLOG_FILE = servList.log\n");
		fprintf(stderr, "\t#Log levels:\n");
		fprintf(stderr, "\t#REDALERT = Red alert\n");
		fprintf(stderr, "\t#DBALERT = Database alert\n");
		fprintf(stderr, "\t#DBMSG = Database message\n");
		fprintf(stderr, "\t#OPALERT = Operation alert\n");
		fprintf(stderr, "\t#OPMSG = Operation message\n");
		fprintf(stderr, "\t#MSG = Just a message\n");
		fprintf(stderr, "\t#DEV = Developer (DEBUG) message\n");
		fprintf(stderr, "\tLOG_LEVEL = REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "\tPAINEL_PASSPHRASE = abcdefghijlmnopqrstuvxz\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(cfgFileLoad(&srvCfg, argv[1], &cfgLineError) == CFGFILE_NOK){
		fprintf(stderr, "Error open/loading (at line: [%d]) configuration file [%s]: [%s].\n", cfgLineError, argv[1], strerror(errno));
		return(-2);
	}

	/* TODO: not implemented yet
	if(cfgFileOpt(&srvCfg, "PAINEL_BIND_ADDRESS", &cfgServerAddress) == CFGFILE_NOK){
		fprintf(stderr, "Config with label PAINEL_BIND_ADDRESS not found into file [%s]! Exit.\n", argv[1]);
		return(-3);
	}
	*/

	if(cfgFileOpt(&srvCfg, "PAINEL_SERVER_PORT", &cfgServerPort) == CFGFILE_NOK){
		fprintf(stderr, "Config with label PAINEL_SERVER_PORT not found into file [%s]! Exit.\n", argv[1]);
		return(-4);
	}

	if(cfgFileOpt(&srvCfg, "LOG_FILE", &cfgLogFile) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_FILE not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}

	if(cfgFileOpt(&srvCfg, "LOG_LEVEL", &cfgLogLevel) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_LEVEL not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}

	if(logCreate(&log, cfgLogFile, cfgLogLevel) == LOG_NOK){
		fprintf(stderr, "[%s %d] Erro criando log! [%s]\n",time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));

		return(-7);
	}

	if(cfgServerPort == NULL || cfgServerPort[0] == '\0'){
		fprintf(stderr, "[%s %d] Port number cannt be null value: [%s]! Exit.\n", time_DDMMYYhhmmss(), getpid(), cfgServerPort);
		return(-9);
	}
	srvListPort = atoi(cfgServerPort);

	if(cfgFileFree(&srvCfg) == CFGFILE_NOK){
		printf("Error at cfgFileFree().\n");
		return(-8);
	}

	p = daemonizeWithoutLock(/*&log*/);
	if(p == (pid_t)PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Cannt daemonize server list!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-9);
	}

	fileName = argv[2];

	logWrite(&log, LOGMUSTLOGIT, "Server List Up! Port: [%s] File: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", srvListPort, fileName, p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1){
		logWrite(&log, LOGOPALERT, "Erro bind: [%s]\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-10);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(srvListPort);

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		logWrite(&log, LOGOPALERT, "Erro bind: [%s]\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-11);
	}

	if(listen(listenfd, 250) != 0){
		logWrite(&log, LOGOPALERT, "Erro listen: [%s]\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-12);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			logWrite(&log, LOGOPALERT, "Erro accept: [%s]\n", strerror(errno));
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			return(-13);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		logWrite(&log, LOGOPMSG, "Connection from [%s], port [%d] at [%s]\n", clientFrom, portFrom, time_DDMMYYhhmmss());

		f = fopen(fileName, "r");
		if(f == NULL){
			logWrite(&log, LOGOPALERT, "Erro abrindo arquivo [%s] para conexao [%s:%d] as [%s]: [%s]. Sent \"PAGE NOT DEFINED!\" to client.\n", fileName, clientFrom, portFrom, time_DDMMYYhhmmss(), strerror(errno));

			send(connfd, HTML_START_PROT1, sizeof(HTML_START_PROT1)-1, /*MBL_DONTWAIT|MBL_MORE*/0);
			send(connfd, HTML_START_PROT2, sizeof(HTML_START_PROT2)-1, /*MBL_DONTWAIT|MBL_MORE*/0);

			send(connfd, "PAGE NOT DEFINED!", sizeof("PAGE NOT DEFINED!"), /*MBL_DONTWAIT|MBL_MORE*/0);

			send(connfd, "\r\n", 2, /*MBL_DONTWAIT*/0);

			shutdown(connfd, SHUT_RDWR);
			close(connfd);

			continue;
		}

		for(i = 0; i <= 10; i++){
			if(i == 10){
				logWrite(&log, LOGOPALERT, "Nao foi liberado o LOCK para o arquivo [%s] em 10 tentativas!\n", fileName);
				logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

				logClose(&log);
				return(-14);
			}

			retLock = html_testHtmlLock(f);

			if(retLock == HTML_FILE_LOCKED){
				logWrite(&log, LOGOPMSG, "Arquivo [%s] locked! Aguardando 3 segundos de 10 tentativas: %d/10\n", fileName, i+1);
				sleep(3);
			}else if(retLock == HTML_FILE_UNLOCKED){
				break;
			}else{
				logWrite(&log, LOGOPALERT, "Erro em testar LOCK no aquivo html [%s]!\n", fileName);
				logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

				logClose(&log);
				return(-15);
			}
		}

		send(connfd, HTML_START_PROT1, sizeof(HTML_START_PROT1)-1, /*MBL_DONTWAIT|MBL_MORE*/0);
		send(connfd, HTML_START_PROT2, sizeof(HTML_START_PROT2)-1, /*MBL_DONTWAIT|MBL_MORE*/0);

		while(!feof(f)){
			memset(msg, '\0', MAXLINE);

			if(fgets(msg, MAXLINE, f) == NULL) break;

			send(connfd, msg, strlen(msg), /*MBL_DONTWAIT|MBL_MORE*/0);
		}

		send(connfd, "\r\n", 2, /*MBL_DONTWAIT*/0);

		shutdown(connfd, SHUT_RDWR);
		close(connfd);
	}

	logWrite(&log, LOGREDALERT, "Terminating application with sucessfully!\n\n");

	logClose(&log);
	return(0);
}
