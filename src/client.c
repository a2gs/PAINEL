/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* client.c
 * Client interface.
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
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"
#include "client.h"

#include "SG_client.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define USER_INPUT_LEN		(400)
#define LOG_CLIENT_FILE    ("client.log")


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* void limparCampoProto(char *campo, int c_from, int c_to)
 *
 * The char '|' is a metacharacter to correct protocol usage. If users uses the metachar,
 * this function replaces to another.
 *
 * INPUT:
 *  campo - message to be cleaned
 *  c_from - Protocol metachar that cannt be used
 *  c_to - Charcter used to replace
 * OUTPUT:
 *  void
 */
void limparCampoProto(char *campo, int c_from, int c_to)
{
	char *c = NULL;

	c = campo;

	for(; c != NULL; ){
		c = strchr(c, '|');

		if(c == NULL) break;
		else          *c = c_to;
	}

	return;
}

tipoPreenchimento_t preencherCampo(const char *pergunta, char *campo, size_t szCampo)
{
	char usrInput[USER_INPUT_LEN + 1] = {0};
	char *c = NULL;

	printf("%s", pergunta);
	fgets(usrInput, USER_INPUT_LEN, stdin);
	c = strchr(usrInput, '\n');
	if(*c == '\n') *c = '\0';

	limparCampoProto(usrInput, '|', '-');

	if(strncmp(LOGOUT_CMD,        usrInput, sizeof(LOGOUT_CMD)  -1) == 0) return(EXITPC);
	else if(strncmp(CORRIGIR_CMD, usrInput, sizeof(CORRIGIR_CMD)-1) == 0) return(CORRIGIRPC);
	else                                                              strncpy(campo, usrInput, ((szCampo <= USER_INPUT_LEN) ? szCampo : USER_INPUT_LEN));

	return(OKPC);
}

/* -------------------------------------------------------------------------------------------------------- */

/* int main(int argc, char *argv[])
 *
 * Client starts.
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
	struct sockaddr_in servaddr;
	char id[DRT_LEN + 1] = {'\0'};
	char level[VALOR_FUNCAO_LEN + 1] = {'\0'};
	char passhash[PASS_SHA256_LEN + 1] = {'\0'};
	tipoUsuario_t usrType = UNDEFINED_USER;

	if(argc != 3){
		fprintf(stderr, "%s <IP_ADDRESS> <PORT>\n", argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if(log_open(LOG_CLIENT_FILE) == NOK){
		fprintf(stderr, "Unable to open/create [%s]!\n", LOG_CLIENT_FILE);
		return(1);
	}
	
	/* gethostbyname() */

	log_write("StartUp Client [%s]! Server: [%s] Port: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), argv[1], argv[2], getPAINELEnvHomeVar());

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		log_write("ERRO socket(): [%s].\n", strerror(errno));
		return(-1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(atoi(argv[2]));

	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0){
		log_write("ERRO inet_pton(): [%s].\n", strerror(errno));
		return(-1);
	}

	if(connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
		log_write("ERRO connect(): [%s].\n", strerror(errno));
		return(-1);
	}

	for(;;){

		if(SG_fazerLogin(id, passhash, level, &usrType) == NOK){
			log_write("EXIT!!!!!!!!\n"); /* TODO */
			break;
		}

		if(usrType == UNDEFINED_USER) continue;

		log_write("[%s] conectado no [%s:%s] at [%s].\n", id, argv[1], argv[2], time_DDMMYYhhmmss());

		if(SG_sendLogin(sockfd, id, passhash, level) == NOK){
			log_write("Invalid user/pass!\n");
			return(-1);
		}

		SG_clientScreen(sockfd, id, level, usrType);

		SG_sendExit(sockfd, id, level);
	}

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return(0);
}
