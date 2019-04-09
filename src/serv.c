/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* serv.c
 * Server creation.
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
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"
#include "db.h"
#include "SG_serv.h"

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOCK_FILE							("servlock")
#define MAX_PATH_RUNNING_LOCKFD		(250)
#define SUBPATH_RUNNING_DATA_SRV		SUBPATH_RUNNING_DATA

typedef struct _userIdent_t{
	char username[DRT_LEN + 1];
	char level[VALOR_FUNCAO_LEN + 1];
	char dateTime[DATA_LEN + 1];
	char passhash[PASS_SHA256_LEN + 1];
}userIdent_t;


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static int lockFd = 0;
static char runnigPath[MAX_PATH_RUNNING_LOCKFD + 1] = {'\0'};
static char runnigLockFdPath[MAX_PATH_RUNNING_LOCKFD + 1] = {'\0'};
static log_t log;


/* ---[DAEMON]--------------------------------------------------------------------------------------------- */
/* *** FUNCTIONS *********************************************************************** */
/* void signal_handler(int sig)
 *
 * Daemon signal handler. Remove the 'process lock file'.
 *
 * INPUT:
 *  sig - Signal number
 * OUTPUT:
 *  void
 */
void signal_handler(int sig)
{
	/* Qualquer sinal recebido ira terminar o server */
	logWrite(&log, LOGMUSTLOGIT, "Got signal [%d] at [%s]!\n", sig, time_DDMMYYhhmmss());

	switch(sig){
		case SIGHUP:
			break;
		case SIGTERM:
			break;
	}

	if(lockf(lockFd, F_ULOCK, 0) < 0){
		logWrite(&log, LOGMUSTLOGIT, "Cannt unlock 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		exit(-1);
	}

	/* Removendo lock */
	shutdown(lockFd, SHUT_RDWR);
	close(lockFd);
	if(unlink(runnigLockFdPath) != 0){
		logWrite(&log, LOGMUSTLOGIT, "Erro deleting lock file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		exit(-1);
	}

	/* Termina servidor */
	exit(0);

	return;
}

/* pid_t daemonize(void)
 *
 * Spawns the process as a daemon.
 * Create a 'process lock file' (guaranteeing only one instance)
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  pid_t - The daemon PID.
 */
pid_t daemonize(void)
{
	pid_t i = 0, father = 0;
	char str[20] = {0};

	father = getppid();
	if(father == 1)
		return((pid_t)NOK);

	i = fork();

	if(i == -1){
		logWrite(&log, LOGMUSTLOGIT, "Erro fork chield process! [%s].\n", strerror(errno));
		return((pid_t)NOK);
	}

	if(i > 0)
		exit(0); /* Pai termina: 0 - Ok */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	/* Criando e travando arquivo de execucao unica */
	snprintf(runnigPath, MAX_PATH_RUNNING_LOCKFD, "%s/%s/", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_SRV);
	if(chdir(runnigPath) == -1){
		logWrite(&log, LOGMUSTLOGIT, "Unable changes to running dir [%s]: [%s].\n", runnigPath, strerror(errno));
		return((pid_t)NOK);
	}

	snprintf(runnigLockFdPath, MAX_PATH_RUNNING_LOCKFD, "%s/%s", runnigPath, LOCK_FILE);
	lockFd = open(runnigLockFdPath, O_RDWR|O_CREAT|O_EXCL, 0640);
	if(lockFd == -1){
		logWrite(&log, LOGMUSTLOGIT, "There is another instance already running. Check [%s] file: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	if(lockf(lockFd, F_TLOCK, 0) < 0){
		logWrite(&log, LOGMUSTLOGIT, "Cannt test 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	if(lockf(lockFd, F_LOCK, 0) < 0){
		logWrite(&log, LOGMUSTLOGIT, "Cannt lock 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	/* Primeira instancia */
	snprintf(str, sizeof(str), "%d\n", getpid());
	write(lockFd, str, strlen(str));

	/* Configurando sinais */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGPIPE, signal_handler);

	return(getpid());
}

/* int parsingLogin(char *msg, userIdent_t *userSession)
 *
 * Parser the login message.
 *
 * INPUT:
 * OUTPUT:
 *  OK - Valid login
 *  NOK - Not valid login
 */
int parsingLogin(char *msg, userIdent_t *userSession)
{
	char *p = NULL;

	/* <COD|> DRT|DATAHORA|FUNCAO|PASSHASH */
	memset(userSession, 0, sizeof(userIdent_t));
	p = msg;

	/* DRT */
	cutter(&p, '|', userSession->username, DRT_LEN);

	/* DATAHORA */
	cutter(&p, '|', userSession->dateTime, DATA_LEN);

	/* FUNCAO */
	cutter(&p, '|', userSession->level, VALOR_FUNCAO_LEN);

	/* PASSHASH */
	cutter(&p, '|', userSession->passhash, PASS_SHA256_LEN);

	return(OK);

#if 0
	char *c1 = NULL;
	char *c2 = NULL;
	size_t fieldSz = 0;

	memset(userSession, 0, sizeof(userIdent_t));

	/* <COD|> DRT|DATAHORA|FUNCAO|PASSHASH */
	c1 = c2 = msg;

	/* DRT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	fieldSz = (size_t) (c2-c1);
	strncpy(userSession->username, c1, ((fieldSz < sizeof(userSession->username)) ? fieldSz : sizeof(userSession->username)-1));
	c1 = c2+1;

	/* DATAHORA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	fieldSz = (size_t) (c2-c1);
	strncpy(userSession->dateTime, c1, ((fieldSz < sizeof(userSession->dateTime)) ? fieldSz : sizeof(userSession->dateTime)-1));
	c1 = c2+1;

	/* FUNCAO */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	fieldSz = (size_t) (c2-c1);
	strncpy(userSession->level, c1, ((fieldSz < sizeof(userSession->level)) ? fieldSz : sizeof(userSession->level)-1));
	c1 = c2+1;

	/* PASSHASH */
	strncpy(userSession->passhash, c1, sizeof(userSession->passhash)-1);

	return(OK);
#endif
}


/* int sendClientResponse(int connfd, int ProtCode, void *data, size_t szData)
 *
 * 
 *
 * INPUT:
 *  connfd - 
 *  ProtCode - 
 *  data - 
 *  szData = 
 * OUTPUT:
 *  OK - Sent ok
 *  NOK - Didnt send
 */
int sendClientResponse(int connfd, int ProtCode, void *data, size_t szData)
{
	char msg[MAXLINE + 1] = {'\0'};

	switch(ProtCode){
		case PROT_COD_LOGIN:
			snprintf(msg, MAXLINE, "%d|%s", PROT_COD_LOGIN, (char *)data);
			break;

		case PROT_COD_LOGOUT:
			break;

		default:
			return(NOK);
	}

	logWrite(&log, LOGDEV, "Sending response to client: [%s].\n", msg);

	if(send(connfd, msg, strlen(msg), 0) == -1){
		logWrite(&log, LOGREDALERT, "ERRO: sendClientResponse(send()) [%s]: [%s].\n", msg, strerror(errno));
		return(NOK);
	}

	return(OK);
}

/* -------------------------------------------------------------------------------------------------------- */

/* int main(int argc, char *argv[])
 *
 * Server starts.
 *
 * INPUT:
 *  argv[1] - Server port
 * OUTPUT (to SO):
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	pid_t p = 0;
	int listenfd = 0, connfd = 0, readRet = 0;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char *msgP = NULL;
	char addStr[255 + 1] = {'\0'};
	char msg[MAXLINE + 1] = {'\0'} /*, *endLine = NULL*/;
	char msgCod[PROT_CODE_LEN + 1] = {'\0'};
	char clientFrom[200] = {'\0'};
	uint16_t portFrom = 0;
	SG_registroDB_t msgCleaned = {0};
	userIdent_t userSession = {0};

	if(argc != 4){
		fprintf(stderr, "[%s %d] Usage:\n%s <PORT> <LOG_FULL_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
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

	if(logCreate(&log, argv[2], argv[3]) == LOG_NOK){                                                         
		fprintf(stderr, "[%s %d] Unable to open/create [%s]! [%s]\n", time_DDMMYYhhmmss(), getpid(), argv[2], strerror(errno));

		return(-2);
	}

	getLogSystem(&log); /* Loading log to business rules */

	p = daemonize();
	if(p == (pid_t)NOK){
		logWrite(&log, LOGMUSTLOGIT, "Cannt daemonize server!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		logClose(&log);
		return(-3);
	}

	logWrite(&log, LOGMUSTLOGIT, "Server Up! Port: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", argv[1], p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	/* colocar aqui:
	 * if(abrir arquivo servlock) == OK
	 * 	avisa que o servers ja esta no ar (ler do arquivo o PID e mostrar) e parar o programa
	 */

	if(dbOpen(NULL, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, &log) == NOK){
		logWrite(&log, LOGOPALERT, "Erro em abrir banco de dados!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		logClose(&log);
		return(-4);
	}

	if(dbCreateAllTables() == NOK){
		logWrite(&log, LOGOPALERT, "Erro em criar tabelas/indices em banco de dados!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		dbClose();
		logClose(&log);
		return(-5);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		logWrite(&log, LOGOPALERT, "Erro bind: [%s].\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		dbClose();
		logClose(&log);
		return(-6);
	}

	if(listen(listenfd, 250) != 0){
		logWrite(&log, LOGOPALERT, "Erro listen: [%s].\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		dbClose();
		logClose(&log);
		return(-7);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			logWrite(&log, LOGOPALERT, "Erro accept: [%s].\n", strerror(errno));
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			dbClose();
			logClose(&log);
			return(-8);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		logWrite(&log, LOGOPMSG, "Connection from [%s], port [%d] at [%s].\n", clientFrom, portFrom, time_DDMMYYhhmmss());
		p = fork();

		if(p == 0){ /* child */
			while(1){
				memset(msg,    '\0', sizeof(msg)   );
				memset(msgCod, '\0', sizeof(msgCod));
				/*szCod = 0;*/

				readRet = recv(connfd, msg, MAXLINE, 0);
				if(readRet == 0){
					logWrite(&log, LOGOPMSG, "End of data from [%s:%d] at [%s].\n", clientFrom, portFrom, time_DDMMYYhhmmss());
					break;
				}

				if(readRet < 0){
					logWrite(&log, LOGOPALERT, "Erro recv(): [%s].\n", strerror(errno));
					break;
				}

				msgP = strrchr(msg, '\n');
				if(msgP != NULL) (*msgP) = '\0';

				logWrite(&log, LOGDEV, "Msg from [%s:%d]: [%s].\n", clientFrom, portFrom, msg);

				/* Capturando o CODIGO da mensagem */
				msgP = msg;
				cutter(&msgP, '|', msgCod, PROT_CODE_LEN);

				/*
				endLine = strchr(msg, '|');
				if(endLine != NULL){
					if(endLine-msg < PROT_CODE_LEN) szCod = endLine-msg;
					else                            szCod = PROT_CODE_LEN;

					strncpy(msgCod, msg, szCod);
					msgCod[szCod] = '\0';
				}else{
					logWrite(&log, LOGOPALERT, "Mensagem de codigo nao reconhecido! [%s].\n", msg);
					continue;
				}
				*/

				switch(atoi(msgCod)){

					case PROT_COD_LOGIN:

						/*if(parsingLogin(&msg[szCod + 1], &userSession) == NOK){*/
						if(parsingLogin(msgP, &userSession) == NOK){
							/* Bad formmated protocol */
							char *loginErrorMsgToClient = "ERRO|login protocol is bad formatted!";

							logWrite(&log, LOGOPALERT, "Login protocol bad formatted [%s]! Disconnecting.\n", msg);
							logWrite(&log, LOGREDALERT, "Terminating application!\n");

							sendClientResponse(connfd, PROT_COD_LOGIN, loginErrorMsgToClient, strlen(loginErrorMsgToClient));

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-9);
						}

						if(SG_checkLogin(userSession.username, userSession.passhash, userSession.level) == NOK){
							char *loginErrorMsgToClient = "ERRO|User/funcion/password didnt find into database!";

							logWrite(&log, LOGOPMSG, "User [%s][%s][%s][%s] not found into database!\n", userSession.username, userSession.level, userSession.passhash, userSession.dateTime);

							if(sendClientResponse(connfd, PROT_COD_LOGIN, loginErrorMsgToClient, strlen(loginErrorMsgToClient)) == NOK){
								logWrite(&log, LOGOPALERT, "Problem sent login error message! Disconnecting.\n");
								logWrite(&log, LOGREDALERT, "Terminating application!\n");

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-10);
							}

						}else{
							char *loginErrorMsgToClient = "OK|User registred into database!";

							logWrite(&log, LOGOPMSG, "Login ok: [%s][%s][%s]\n", userSession.username, userSession.level, userSession.dateTime);

							if(sendClientResponse(connfd, PROT_COD_LOGIN, loginErrorMsgToClient, strlen(loginErrorMsgToClient)) == NOK){
								logWrite(&log, LOGOPALERT, "Problem sent login success message! Disconnecting.\n");
								logWrite(&log, LOGREDALERT, "Terminating application!\n");

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-11);
							}

							memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

							SG_fillInDataInsertLogin(userSession.username, userSession.level, userSession.dateTime, clientFrom, portFrom, &msgCleaned);

							if(SG_db_inserting(&msgCleaned) == NOK){
								logWrite(&log, LOGDBALERT, "Error inserting user login database register [%s:%d]: [%s][%s][%s]! But it is working (logged) at its terminal...\n", clientFrom, portFrom, userSession.username, userSession.level, userSession.dateTime);
							}
						}
						break;

					case PROT_COD_LOGOUT:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));
						/* TODO: IMPLEMENTAR ESTE COMANDO */

						logWrite(&log, LOGOPALERT, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					case PROT_COD_INSREG:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

						if(SG_parsingDataInsertRegistro(msgP, clientFrom, portFrom, &msgCleaned) == NOK){
							logWrite(&log, LOGOPALERT, "PARSING INSERT ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO: melhorar mensagem */
							continue;
						}else{
							if(SG_db_inserting(&msgCleaned) == NOK){
								logWrite(&log, LOGDBALERT, "INSERT ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO: melhorar mensagem */
							}
						}
						break;

					/*
					case PROT_COD_SERCMD:
						log_write("Codigo [%s] ainda nao implementado!\n", msgCod);
						break;
					*/

					case PROT_COD_CLICMD:
						logWrite(&log, LOGOPALERT, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					default:
						logWrite(&log, LOGOPALERT, "Nao ha implementacao de tratamento para codigo de mensagem: [%s]!\n", msgCod);
						break;
				}
			}

			break; /* for() */

		}else if(p == -1)
			logWrite(&log, LOGOPALERT, "Erro fork: [%s].\n", strerror(errno));
	}

	logWrite(&log, LOGREDALERT, "Terminating application with sucessfully!\n");

	dbClose();
	logClose(&log);
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	return(0);
}
