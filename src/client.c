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
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "util.h"
#include "client.h"

#include "SG_client.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define USER_INPUT_LEN		(400)
#define LOG_CLIENT_FILE    ("client.log")
#define STRADDR_SZ			(50)


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
	if(c != NULL) *c = '\0';

	limparCampoProto(usrInput, '|', '-');

	if(strncmp(LOGOUT_CMD,        usrInput, sizeof(LOGOUT_CMD)  -1) == 0) return(EXITPC);
	else if(strncmp(CORRIGIR_CMD, usrInput, sizeof(CORRIGIR_CMD)-1) == 0) return(CORRIGIRPC);
	else                                                                  strncpy(campo, usrInput, ((szCampo <= USER_INPUT_LEN) ? szCampo : USER_INPUT_LEN));

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
	struct addrinfo hints, *res = NULL, *rp = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	char strAddr[STRADDR_SZ + 1] = {'\0'};
	void *pAddr = NULL;
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
		return(-1);
	}

	log_write("StartUp Client [%s]! Server: [%s] Port: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), argv[1], argv[2], getPAINELEnvHomeVar());

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(argv[1], argv[2], &hints, &res);
	if(errGetAddrInfoCode != 0){
		log_write("ERRO: getaddrinfo() [%s].\n", gai_strerror(errGetAddrInfoCode));
		return(-1);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			log_write("ERRO: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		log_write("Trying connect to [%s/%s:%s].\n", rp->ai_canonname, strAddr, argv[2]);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		log_write("ERRO: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, argv[2], strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		log_write("ERRO: Unable connect to any address.\n");
		return(-1);
	}

	freeaddrinfo(res);

	for(;;){

		if(SG_fazerLogin(id, passhash, level, &usrType) == NOK){
			log_write("EXIT!!!!!!!!\n"); /* TODO: melhorar mensagem */
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
