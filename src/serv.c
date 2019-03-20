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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"
#include "SG_serv.h"
/*
#include "serv.h"
*/


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOCK_FILE							("servlock")
#define MAX_PATH_RUNNING_LOCKFD		(250)
#define SUBPATH_RUNNING_DATA_SRV		SUBPATH_RUNNING_DATA
#define LOG_SERV_FILE					("server.log")


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static int lockFd = 0;
static char runnigPath[MAX_PATH_RUNNING_LOCKFD + 1] = {'\0'};
static char runnigLockFdPath[MAX_PATH_RUNNING_LOCKFD + 1] = {'\0'};


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
	log_write("Got signal [%d] at [%s]!\n", sig, time_DDMMYYhhmmss());

	switch(sig){
		case SIGHUP:
			break;
		case SIGTERM:
			break;
	}

	if(lockf(lockFd, F_ULOCK, 0) < 0){
		log_write("Cannt unlock 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		exit(-1);
	}

	/* Removendo lock */
	shutdown(lockFd, SHUT_RDWR);
	close(lockFd);
	if(unlink(runnigLockFdPath) != 0){
		log_write("Erro deleting lock file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
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
		log_write("Erro fork chield process! [%s].\n", strerror(errno));
		return((pid_t)NOK);
	}

	if(i > 0)
		exit(0); /* Pai termina: 0 - Ok */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	/* Criando e travando arquivo de execucao unica */
	snprintf(runnigPath, MAX_PATH_RUNNING_LOCKFD, "%s/%s/", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA);
	if(chdir(runnigPath) == -1){
		log_write("Unable changes to running dir [%s]: [%s].\n", runnigPath, strerror(errno));
		return((pid_t)NOK);
	}

	snprintf(runnigLockFdPath, MAX_PATH_RUNNING_LOCKFD, "%s/%s", runnigPath, LOCK_FILE);
	lockFd = open(runnigLockFdPath, O_RDWR|O_CREAT|O_EXCL, 0640);
	if(lockFd == -1){
		log_write("There is another instance already running. Check [%s] file: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	if(lockf(lockFd, F_TLOCK, 0) < 0){
		log_write("Cannt test 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	if(lockf(lockFd, F_LOCK, 0) < 0){
		log_write("Cannt lock 'only one instance' file [%s]: [%s].\n", runnigLockFdPath, strerror(errno));
		return((pid_t)NOK);
	}

	/* Primeira instancia */
	sprintf(str, "%d\n", getpid());
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

/* int checkLogin(char *msg)
 *
 * Parser the login message.
 *
 * INPUT:
 *  msg - Message
 * OUTPUT:
 *  OK - Valid login
 *  NOK - Not valid login
 */
int checkLogin(char *msg)
{
	char user[DRT_LEN + 1] = {'\0'};
	char func[VALOR_FUNCAO_LEN + 1] = {'\0'};
	char passhash[PASS_SHA256_LEN + 1] = {'\0'};
	char *c1 = NULL;
	char *c2 = NULL;

	/* <COD|> DRT|DATAHORA|FUNCAO */
	c1 = c2 = msg;

	/* DRT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(user, c1, ((c2-c1 < DRT_LEN) ? c2-c1 : DRT_LEN));
	c1 = c2+1;

	/* DATAHORA - unused */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	c1 = c2+1;

	/* FUNCAO */
	strcpy(func, c1);

	if(SG_checkLogin(user, passhash, func) == NOK){
		return(NOK);
	}

	return(OK);
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
	char addStr[255 + 1] = {'\0'};
	char msg[MAXLINE + 1] = {'\0'}, *endLine = NULL;
	char msgCod[PROT_CODE_LEN + 1] = {'\0'};
	int szCod = 0;
	char clientFrom[200] = {'\0'};
	uint16_t portFrom = 0;
	SG_registroDB_t msgCleaned = {0};

	if(argc != 2){
		fprintf(stderr, "%s PORT\n", argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(1);
	}

	if(log_open(LOG_SERV_FILE) == NOK){
		fprintf(stderr, "Unable to open/create [%s]! [%s]\n", LOG_SERV_FILE, strerror(errno));
		return(1);
	}

	p = daemonize();
	if(p == (pid_t)NOK){
		log_write("Cannt daemonize server!\n");
		return(1);
	}

	log_write("Server Up! Port: [%s] PID: [%d] Date: [%s] PAINEL Home: [%s].\n", argv[1], p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar());

	/* colocar aqui:
	 * if(abrir arquivo servlock) == OK
	 * 	avisa que o servers ja esta no ar (ler do arquivo o PID e mostrar) e parar o programa
	 */

	if(SG_db_open_or_create() == NOK){
		log_write("Erro em abrir/criar banco de dados!\n");
		return(1);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		log_write("Erro bind: [%s].\n", strerror(errno));
		return(1);
	}

	if(listen(listenfd, 250) != 0){
		log_write("Erro listen: [%s].\n", strerror(errno));
		return(1);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			log_write("Erro accept: [%s].\n", strerror(errno));
			return(1);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		log_write("Connection from [%s], port [%d] at [%s].\n", clientFrom, portFrom, time_DDMMYYhhmmss());
		p = fork();

		if(p == 0){ /* child */
			while(1){
				memset(msg,    '\0', sizeof(msg)   );
				memset(msgCod, '\0', sizeof(msgCod));
				szCod = 0;

				readRet = recv(connfd, msg, MAXLINE, 0);
				if(readRet == 0){
					log_write("End of data from [%s:%d] at [%s].\n", clientFrom, portFrom, time_DDMMYYhhmmss());
					break;
				}

				if(readRet < 0){
					log_write("Erro recv(): [%s].\n", strerror(errno));
					break;
				}

				endLine = strrchr(msg, '\n');
				if(endLine != NULL) (*endLine) = '\0';

				log_write("Msg from [%s:%d]: [%s].\n", clientFrom, portFrom, msg);

				/* Capturando o CODIGO da mensagem */
				endLine = strchr(msg, '|');
				if(endLine != NULL){
					if(endLine-msg < PROT_CODE_LEN) szCod = endLine-msg;
					else                            szCod = PROT_CODE_LEN;

					strncpy(msgCod, msg, szCod);
					msgCod[szCod] = '\0';
				}else{
					log_write("Mensagem de codigo nao reconhecido! [%s].\n", msg);
					continue;
				}

				switch(atoi(msgCod)){

					case PROT_COD_LOGIN:

						if(checkLogin(&msg[szCod + 1]) == NOK){
							char *loginErrorMsgToClient = "ERRO|User/funcion/password didnt find into database!";
							log_write("USUARIO NAO VALIDADO!\n"); /* TODO: melhorar */

							if(sendClientResponse(connfd, PROT_COD_LOGIN, loginErrorMsgToClient, strlen(loginErrorMsgToClient)) == NOK){
							}

						}else{
							char *loginErrorMsgToClient = "OK|Userregistred into database!";
							log_write("USUARIO VALIDADO!\n"); /* TODO: melhorar */

							if(sendClientResponse(connfd, PROT_COD_LOGIN, loginErrorMsgToClient, strlen(loginErrorMsgToClient)) == NOK){
							}

							memset(&msgCleaned, 0, sizeof(SG_registroDB_t));
							/* TODO: chamar SG_db_inserting() com msgCleaned devidamente populado com dados do momento do login */
						}
						break;

					case PROT_COD_LOGOUT:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));
						/* TODO */

						log_write("Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					case PROT_COD_INSREG:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

						if(SG_parsingDataInsertRegistro(&msg[szCod + 1], clientFrom, portFrom, &msgCleaned) == NOK){

							log_write("PARSING ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO */
							continue;

						}else{

							if(SG_db_inserting(&msgCleaned) == NOK){
								log_write("INSERT ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO */
							}

						}
						break;

					/*
					case PROT_COD_SERCMD:
						log_write("Codigo [%s] ainda nao implementado!\n", msgCod);
						break;
					*/

					case PROT_COD_CLICMD:
						log_write("Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					default:
						log_write("Nao ha implementacao de tratamento para codigo de mensagem: [%s]!\n", msgCod);
						break;
				}
			}

			break; /* for() */

		}else if(p == -1)
			log_write("Erro fork: [%s].\n", strerror(errno));
	}

	SG_db_close();
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	return(0);
}
