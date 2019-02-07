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

#include "sc.h"
#include "serv.h"

#include "SG_serv.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define RUNNING_DIR  ("./")
#define LOCK_FILE    ("./servlock")


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static int lockFd;


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
	log_write(LOG_SERV_FILE, "Got signal [%d] at [%s]!\n", sig, time_DDMMYYhhmmss());

	switch(sig){
		case SIGHUP:
			break;
		case SIGTERM:
			break;
	}

	if(lockf(lockFd, F_ULOCK, 0) < 0){
		log_write(LOG_SERV_FILE, "Cannt unlock 'only one instance' file\n");
		exit(-1);
	}

	shutdown(lockFd, SHUT_RDWR);
	close(lockFd);
	if(unlink(LOCK_FILE) != 0){
		log_write(LOG_SERV_FILE, "Erro deleting lock file [%s].\n", LOCK_FILE);
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
	char str[10];

	father = getppid();
	if(father == 1)
		return((pid_t)NOK);

	i = fork();

	if(i == -1){
		log_write(LOG_SERV_FILE, "Erro fork chield process! [%s]\n", strerror(errno));
		return((pid_t)NOK);
	}

	if(i > 0)
		exit(1); /* Pai termina */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	chdir(RUNNING_DIR);

	/* Criando e travando arquivo de execucao unica */
	lockFd = open(LOCK_FILE, O_RDWR|O_CREAT|O_EXCL, 0640);
	if(lockFd == -1){
		log_write(LOG_SERV_FILE, "There is another instance already running\n");
		exit((pid_t)NOK);
	}

	if(lockf(lockFd, F_TLOCK, 0) < 0){
		log_write(LOG_SERV_FILE, "Cannt test 'only one instance' file\n");
		return((pid_t)NOK);
	}

	if(lockf(lockFd, F_LOCK, 0) < 0){
		log_write(LOG_SERV_FILE, "Cannt lock 'only one instance' file\n");
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
	char user[DRT_LEN] = {'\0'};
	char func[VALOR_FUNCAO_LEN] = {'\0'};
	char passhash[PASS_SHA256_LEN] = {'\0'};
	char *c1 = NULL;
	char *c2 = NULL;

	/* <COD|> DRT|DATAHORA|FUNCAO */
	c1 = c2 = msg;

	/* DRT */
	c2 = strchr(c1, '|');
	strncpy(user, c1, c2-c1);
	c1 = c2+1;

	/* DATAHORA - unused */
	c2 = strchr(c1, '|');
	c1 = c2+1;

	/* FUNCAO */
	strcpy(func, c1);

	if(SG_checkLogin(user, passhash, func) == NOK){
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
	char msg[MAXLINE] = {'\0'}, *endLine = NULL;
	char msgCod[PROT_CODE_LEN+1] = {'\0'};
	int szCod = 0;
	char clientFrom[200] = {'\0'};
	uint16_t portFrom = 0;
	SG_registroDB_t msgCleaned = {0};
	FILE *log = NULL;

	if(argc != 2){
		fprintf(stderr, "%s PORT\n", argv[0]);
		return(1);
	}

	if(log_open(LOG_SERV_FILE, log) == NOK){
		fprintf(stderr, "Unable to open/create [%s]! [%s]\n", LOG_SERV_FILE, strerror(errno));
		return(1);
	}

	p = daemonize();
	if(p == (pid_t)NOK){
		log_write(LOG_SERV_FILE, "Cannt daemonize server!\n");
		return(1);
	}

	log_write(LOG_SERV_FILE, "Server Up! Port [%s] PID [%d] [%s]!\n", argv[1], p, time_DDMMYYhhmmss());

	/* colocar aqui:
	 * if(abrir arquivo servlock) == OK
	 * 	avisa que o servers ja esta no ar (ler do arquivo o PID e mostrar) e parar o programa
	 */

	if(SG_db_open_or_create() == NOK){
		log_write(LOG_SERV_FILE, "Erro em abrir/criar banco de dados!\n");
		return(1);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
		log_write(LOG_SERV_FILE, "Erro bind: [%s]\n", strerror(errno));
		return(1);
	}

	if(listen(listenfd, 250) != 0){
		log_write(LOG_SERV_FILE, "Erro listen: [%s]\n", strerror(errno));
		return(1);
	}

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		if(connfd == -1){
			log_write(LOG_SERV_FILE, "Erro accept: [%s]\n", strerror(errno));
			return(1);
		}

		strcpy(clientFrom, inet_ntop(AF_INET, &cliaddr.sin_addr, addStr, sizeof(addStr)));
		portFrom = ntohs(cliaddr.sin_port);
		log_write(LOG_SERV_FILE, "Connection from [%s], port [%d] at [%s]\n", clientFrom, portFrom, time_DDMMYYhhmmss());
		p = fork();

		if(p == 0){ /* child */
			while(1){
				memset(msg,    '\0', MAXLINE      );
				memset(msgCod, '\0', PROT_CODE_LEN);
				szCod = 0;

				readRet = recv(connfd, msg, MAXLINE, 0);
				if(readRet == 0){
					log_write(LOG_SERV_FILE, "End of data from [%s:%d] at [%s]\n", clientFrom, portFrom, time_DDMMYYhhmmss());
					break;
				}

				if(readRet < 0){
					log_write(LOG_SERV_FILE, "Erro recv(): [%s]\n", strerror(errno));
					break;
				}

				endLine = strrchr(msg, '\n');
				if(endLine != NULL) (*endLine) = '\0';

				log_write(LOG_SERV_FILE, "Msg from [%s:%d]: [%s]\n", clientFrom, portFrom, msg);

				/* Capturando o CODIGO da mensagem */
				endLine = strchr(msg, '|');
				if(endLine != NULL){
					if(msg-endLine < PROT_CODE_LEN) szCod = endLine-msg;
					else                            szCod = PROT_CODE_LEN;

					strncpy(msgCod, msg, szCod);
					msgCod[szCod] = '\0';
				}else{
					log_write(LOG_SERV_FILE, "Mensagem de codigo nao reconhecido! [%s]\n", msg);
					continue;
				}

				switch(atoi(msgCod)){

					case PROT_COD_LOGIN:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

						if(checkLogin(&msg[szCod+1]) == NOK){
							log_write(LOG_SERV_FILE, "USUARIO NAO VALIDADO!\n");
							/* TODO: ENVIAR RESPOSTA DE USER NOT AUTH */
						}else{
							/* TODO: chamar SG_db_inserting() com msgCleaned devidamente populado */
						}
						break;

					case PROT_COD_LOGOUT:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));
						/* TODO */

						log_write(LOG_SERV_FILE, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					case PROT_COD_INSREG:
						memset(&msgCleaned, 0, sizeof(SG_registroDB_t));

						if(SG_parsingDataInsertRegistro(&msg[szCod+1], clientFrom, portFrom, &msgCleaned) == NOK){

							log_write(LOG_SERV_FILE, "PARSING ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO */
							continue;

						}else{

							if(SG_db_inserting(&msgCleaned) == NOK){
								log_write(LOG_SERV_FILE, "INSERT ERROR [%s:%d]: [%s]!\n", clientFrom, portFrom, msg); /* TODO */
							}

						}
						break;

					case PROT_COD_SERCMD:
						log_write(LOG_SERV_FILE, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					case PROT_COD_CLICMD:
						log_write(LOG_SERV_FILE, "Codigo [%s] ainda nao implementado!\n", msgCod);
						break;

					default:
						log_write(LOG_SERV_FILE, "Nao ha implementacao de tratamento para codigo de mensagem: [%s]!\n", msgCod);
						break;
				}
			}

			break; /* for() */

		}else if(p == -1)
			log_write(LOG_SERV_FILE, "Erro fork: [%s]\n", strerror(errno));
	}

	SG_db_close();
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	return(0);
}
