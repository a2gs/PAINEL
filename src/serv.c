/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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
#include "util_network.h"
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
	char passhash[PASS_SHA256_ASCII_LEN + 1];
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
		return((pid_t)PAINEL_NOK);

	i = fork();

	if(i == -1){
		logWrite(&log, LOGMUSTLOGIT, "Erro fork chield process! [%s].\n", strerror(errno));
		return((pid_t)PAINEL_NOK);
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
		return((pid_t)PAINEL_NOK);
	}

	snprintf(runnigLockFdPath, MAX_PATH_RUNNING_LOCKFD, "%s/%s", runnigPath, LOCK_FILE);
	lockFd = open(runnigLockFdPath, O_RDWR|O_CREAT|O_EXCL, 0640);
	if(lockFd == -1){
		logWrite(&log, LOGMUSTLOGIT, "There is another instance already running. Check [%s] file: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)PAINEL_NOK);
	}

	if(lockf(lockFd, F_TLOCK, 0) < 0){
		logWrite(&log, LOGMUSTLOGIT, "Cannt test 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)PAINEL_NOK);
	}

	if(lockf(lockFd, F_LOCK, 0) < 0){
		logWrite(&log, LOGMUSTLOGIT, "Cannt lock 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)PAINEL_NOK);
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

/* int parsingLogout(char *msg, userIdent_t *userSession)
 *
 * Parser the login message.
 *
 * INPUT:
 * OUTPUT:
 *  PAINEL_OK - Valid login
 *  PAINEL_NOK - Not valid login
 */
int parsingLogout(char *msg, userIdent_t *userSession)
{
	char *p = NULL;

	/* <COD|> DRT|DATAHORA|FUNCAO */
	memset(userSession, 0, sizeof(userIdent_t));
	p = msg;

	/* DRT */
	cutter(&p, '|', userSession->username, DRT_LEN);

	/* DATAHORA */
	cutter(&p, '|', userSession->dateTime, DATA_LEN);

	/* FUNCAO */
	cutter(&p, '|', userSession->level, VALOR_FUNCAO_LEN);

	return(PAINEL_OK);
}

/* int parsingLogin(char *msg, userIdent_t *userSession)
 *
 * Parser the login message.
 *
 * INPUT:
 * OUTPUT:
 *  PAINEL_OK - Valid login
 *  PAINEL_NOK - Not valid login
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
	cutter(&p, '|', userSession->passhash, PASS_SHA256_ASCII_LEN);

	return(PAINEL_OK);
}

int parsingPing(char *msg)
{
	/* At the moment, I dont see any information to be sent from client to server (or vice-versa) into 'msg' field. Just receive PROT_COD_PING and response */
	return(PAINEL_OK);
}


/* int sendClientResponse(int connfd, int ProtCode, void *data, size_t szData)
 *
 * 
 *
 * INPUT:
 *  connfd - 
 *  ProtCode - 
 *  data - NULL TERMINATED!!
 * OUTPUT:
 *  PAINEL_OK - Sent ok
 *  PAINEL_NOK - Didnt send
 */
int sendClientResponse(int connfd, int ProtCode, void *data)
{
	char msg[MAXLINE + 1] = {'\0'};
	uint32_t msgNetOrderSz = 0, msgHostOderSz = 0;

	switch(ProtCode){
		case PROT_COD_PING:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_PING, (char *)data);
			break;

		case PROT_COD_LOGIN:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_LOGIN, (char *)data);
			break;

		case PROT_COD_LOGOUT:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_LOGOUT, (char *)data);
			break;

		case PROT_COD_IFACE:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_IFACE, (char *)data);
			break;

		case PROT_COD_LEVELS:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_LEVELS, (char *)data);
			break;

		case PROT_COD_INSREG:
			msgHostOderSz = snprintf(msg, MAXLINE, "%d|%s", PROT_COD_INSREG, (char *)data);
			break;

		default:
			return(PAINEL_NOK);
	}

	msgNetOrderSz = htonl(msgHostOderSz);
	send(connfd, &msgNetOrderSz, 4, 0); /* Sending the message size. 4 bytes at the begin */

	logWrite(&log, LOGDEV, "Sending response to client: [%s][%lu].\n", msg, msgHostOderSz);

	if(send(connfd, msg, msgHostOderSz, 0) == -1){
		logWrite(&log, LOGREDALERT, "ERRO: sendClientResponse(send()) [%s]: [%s].\n", msg, strerror(errno));
		return(PAINEL_NOK);
	}

	return(PAINEL_OK);
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
	int listenfd = 0, connfd = 0/*, readRet = 0*/;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char *msgP = NULL;
	char addStr[255 + 1] = {'\0'};
	char msg[MAXLINE + 1] = {'\0'} /*, *endLine = NULL*/;
	char msgCod[PROT_CODE_LEN + 1] = {'\0'};
	char clientFrom[200] = {'\0'};
	char *msgBackToClient = NULL;
	uint16_t portFrom = 0;
	SG_registroDB_t msgCleaned = {0};
	size_t srSz = 0;
	int recvError = 0;
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

	getLogSystem_SGServer(&log); /* Loading log to business rules */
	getLogSystem_Util(&log); /* Loading log to util functions */
	getLogSystem_DB(&log); /* Loading log to DB functions */

	p = daemonize();
	if(p == (pid_t)PAINEL_NOK){
		logWrite(&log, LOGMUSTLOGIT, "Cannt daemonize server!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-3);
	}

	logWrite(&log, LOGMUSTLOGIT, "Server Up! Port: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", argv[1], p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	/* colocar aqui:
	 * if(abrir arquivo servlock) == PAINEL_OK
	 * 	avisa que o servers ja esta no ar (ler do arquivo o PID e mostrar) e parar o programa
	 */

	if(dbOpen(NULL, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro em abrir banco de dados!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-4);
	}

	if(dbCreateAllTables() == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro em criar tabelas/indices em banco de dados!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

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
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		dbClose();
		logClose(&log);
		return(-6);
	}

	if(listen(listenfd, 250) != 0){
		logWrite(&log, LOGOPALERT, "Erro listen: [%s].\n", strerror(errno));
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		dbClose();
		logClose(&log);
		return(-7);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			logWrite(&log, LOGOPALERT, "Erro accept: [%s].\n", strerror(errno));
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

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
				srSz = 0; recvError = 0;
				msgBackToClient = NULL;

				/* Reading the message */
				if(recvFromNet(connfd, msg, MAXLINE, &srSz, &recvError) == PAINEL_NOK){
					logWrite(&log, LOGOPALERT, "Erro server receving(): [%s].\n", strerror(recvError));
					break;
				}

				if(srSz == 0){
					logWrite(&log, LOGOPMSG, "End of data from [%s:%d] at [%s].\n", clientFrom, portFrom, time_DDMMYYhhmmss());
					break;
				}

				logWrite(&log, LOGDEV, "Msg from [%s:%d]: Raw msg [%s] [%lu]B.\n", clientFrom, portFrom, msg, srSz);

				/* Capturando o CODIGO da mensagem */
				msgP = msg;
				cutter(&msgP, '|', msgCod, PROT_CODE_LEN);

				switch(atoi(msgCod)){

					case PROT_COD_PING:
						msgBackToClient = "PONG";

						if(parsingPing(msgP) == PAINEL_NOK){
							/* At the moment, I dont see any information to be sent from client to server (or vice-versa) into 'msg' field. Just receive PROT_COD_PING and response */
						}

						logWrite(&log, LOGOPMSG, "Ping from [%s:%d] at [%s]. Sending PONG.\n", clientFrom, portFrom, time_DDMMYYhhmmss());

						if(sendClientResponse(connfd, PROT_COD_PING, msgBackToClient) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "Problem sent ping error message! Disconnecting.\n");
							logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-9);
						}

						break;

					case PROT_COD_LOGIN:

						if(parsingLogin(msgP, &userSession) == PAINEL_NOK){
							/* Bad formmated protocol */
							msgBackToClient = "ERRO|login protocol is bad formatted!";

							logWrite(&log, LOGOPALERT, "Login protocol bad formatted [%s]! Disconnecting.\n", msg);
							logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

							sendClientResponse(connfd, PROT_COD_LOGIN, msgBackToClient);

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-10);
						}

						if(SG_checkLogin(userSession.username, userSession.passhash, userSession.level) == PAINEL_NOK){
							msgBackToClient = "ERRO|User/funcion/password didnt find into database!";

							logWrite(&log, LOGOPMSG, "User [%s][%s][%s][%s] not found into database!\n", userSession.username, userSession.level, userSession.passhash, userSession.dateTime);

							if(sendClientResponse(connfd, PROT_COD_LOGIN, msgBackToClient) == PAINEL_NOK){
								logWrite(&log, LOGOPALERT, "Problem sent login error message! Disconnecting.\n");
								logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-11);
							}

						}else{
							logWrite(&log, LOGOPMSG, "Login ok: [%s][%s][%s]\n", userSession.username, userSession.level, userSession.dateTime);

							memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

							SG_fillInDataInsertLogin(userSession.username, userSession.level, userSession.dateTime, clientFrom, portFrom, &msgCleaned);

							if(SG_db_inserting(&msgCleaned) == PAINEL_NOK){
								logWrite(&log, LOGDBALERT, "Error inserting user login database register [%s:%d]: [%s][%s][%s]! But it is working (logged) at its terminal...\n", clientFrom, portFrom, userSession.username, userSession.level, userSession.dateTime);
								msgBackToClient = "OK|BUT USER WAS NOT REGISTERED LOGIN INTO DATABASE (server insert error)!";
							}else{
								msgBackToClient = "OK|User registred login into database!";
							}

							if(sendClientResponse(connfd, PROT_COD_LOGIN, msgBackToClient) == PAINEL_NOK){
								logWrite(&log, LOGOPALERT, "Problem sent login success message! Disconnecting.\n");
								logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-12);
							}
						}
						break;

					case PROT_COD_LOGOUT:

						if(parsingLogout(msgP, &userSession) == PAINEL_NOK){
							/* Bad formmated protocol */
							msgBackToClient = "ERRO|logout protocol is bad formatted!";

							logWrite(&log, LOGOPALERT, "Login protocol bad formatted [%s]! Disconnecting.\n", msg);
							logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

							sendClientResponse(connfd, PROT_COD_LOGOUT, msgBackToClient);

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-13);

						}else{
							memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

							SG_fillInDataInsertLogout(userSession.username, userSession.level, userSession.dateTime, clientFrom, portFrom, &msgCleaned);

							if(SG_db_inserting(&msgCleaned) == PAINEL_NOK){
								logWrite(&log, LOGDBALERT, "Error inserting user logout database register [%s:%d]: [%s][%s][%s]! But it is working (logged) at its terminal...\n", clientFrom, portFrom, userSession.username, userSession.level, userSession.dateTime);
								msgBackToClient = "OK|BUT USER WAS NOT REGISTERED LOGOUT INTO DATABASE (server insert error)!";
							}else{
								msgBackToClient = "OK|User registered logout into database!";
							}

							if(sendClientResponse(connfd, PROT_COD_LOGOUT, msgBackToClient) == PAINEL_NOK){
								logWrite(&log, LOGOPALERT, "Problem sent logout success message! Disconnecting.\n");
								logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-14);
							}
						}

						break;

					case PROT_COD_INSREG:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

						if(SG_parsingDataInsertRegistro(msgP, clientFrom, portFrom, &msgCleaned) == PAINEL_NOK){
							msgBackToClient = "ERRO|Protocol INSERT REGSITER unable to parse!";

							logWrite(&log, LOGOPALERT, "ERRO: PARSING INSERT REGISTER FROM CLIENT [%s:%d]: [%s]!\n", clientFrom, portFrom, msgP);

							if(sendClientResponse(connfd, PROT_COD_INSREG, msgBackToClient) == PAINEL_NOK){
								logWrite(&log, LOGOPALERT, "Message error parsing sent back to client [%s:%d] [%s]!\n", clientFrom, portFrom, msgBackToClient);

								dbClose();
								shutdown(connfd, SHUT_RDWR);
								close(connfd);
								logClose(&log);

								return(-15);
							}

							continue;
						}else{
							if(SG_db_inserting(&msgCleaned) == PAINEL_NOK){
								msgBackToClient = "ERRO|Unable to insert register into database!";

								logWrite(&log, LOGDBALERT, "ERRO: INSERT REGISTER FROM CLIENT [%s:%d]: [%s]!\n", clientFrom, portFrom, msg);

								if(sendClientResponse(connfd, PROT_COD_INSREG, msgBackToClient) == PAINEL_NOK){
									logWrite(&log, LOGOPALERT, "Message error insert sent back to client [%s:%d] [%s]!\n", clientFrom, portFrom, msgBackToClient);

									dbClose();
									shutdown(connfd, SHUT_RDWR);
									close(connfd);
									logClose(&log);

									return(-16);
								}
							}
						}

						msgBackToClient = "OK|Register inserted!";

						if(sendClientResponse(connfd, PROT_COD_INSREG, msgBackToClient) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "Error sent fail process message back to client [%s:%d] [%s]!\n", clientFrom, portFrom, msgBackToClient);

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-17);
						}

						break;

					case PROT_COD_IFACE:

						if(strcmp(userSession.username, "") == 0){ /* Session/user not logged */
							logWrite(&log, LOGOPALERT, "PROT_COD_IFACE with user not logged! Disconnected.\n");

							dbClose();
							logClose(&log);
							shutdown(connfd, SHUT_RDWR);
							close(connfd);

							return(-18);
						}

						msgBackToClient = msg;
						memset(msg, '\0', sizeof(msg));

						if(SG_getUserIFace(msgBackToClient, sizeof(msg), userSession.level) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "PROT_COD_IFACE SG_getUserIFace() error! Disconnected.\n");

							dbClose();
							logClose(&log);
							shutdown(connfd, SHUT_RDWR);
							close(connfd);

							return(-19);
						}

						if(sendClientResponse(connfd, PROT_COD_IFACE, msgBackToClient) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "Error sent fail iface process message back to client [%s:%d] [%s]!\n", clientFrom, portFrom, msgBackToClient);

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-20);
						}

						break;

					case PROT_COD_LEVELS:

						msgBackToClient = msg;
						memset(msg, '\0', sizeof(msg));

						if(SG_getLevels(msgBackToClient, sizeof(msg)) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "PROT_COD_LEVELS SG_getLevels() error! Disconnected.\n");

							dbClose();
							logClose(&log);
							shutdown(connfd, SHUT_RDWR);
							close(connfd);

							return(-21);
						}

						if(sendClientResponse(connfd, PROT_COD_LEVELS, msgBackToClient) == PAINEL_NOK){
							logWrite(&log, LOGOPALERT, "Error sent fail levels process message back to client [%s:%d] [%s]!\n", clientFrom, portFrom, msgBackToClient);

							dbClose();
							shutdown(connfd, SHUT_RDWR);
							close(connfd);
							logClose(&log);

							return(-22);
						}

						break;

					case PROT_COD_SERCMD:
						logWrite(&log, LOGOPALERT, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

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

	logWrite(&log, LOGREDALERT, "Terminating application with sucessfully!\n\n");

	dbClose();
	logClose(&log);
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	return(0);
}
