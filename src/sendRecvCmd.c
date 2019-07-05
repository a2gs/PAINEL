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

#include "util.h"
#include "util_network.h"


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
	FILE *f = NULL;
	char line[MAXLINE] = {'\0'};
	char *c = NULL;
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

	if(connectSrvPainel(argv[1], argv[2]) == PAINEL_NOK){
		logWrite(NULL, LOGOPALERT, "Erro conetando ao servidor PAINEL [%s:%s].\n", argv[1], argv[2]);
		return(-1);
	}

	f = fopen(argv[3], "r");
	if(f == NULL){
		printf("Unable to open file: [%s]\n", strerror(errno));
		return(-1);
	}

	for(; fgets(line, MAXLINE, f) != NULL; ){
		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		fprintf(stderr, "Sending: [%s] Bytes: [%ld]\n", line, strlen(line));
		if(sendToNet(getSocket(), line, strlen(line), &srError) == PAINEL_NOK){
			fprintf(stderr, "sendToNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}

		if(recvFromNet(getSocket(), line, MAXLINE, &recvSz, &srError) == PAINEL_NOK){
			fprintf(stderr, "recvFromNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}
		fprintf(stderr, "Received: [%s] Bytes: [%ld].\n", line, recvSz);

	}

	fclose(f);

	fprintf(stderr, "Terminating application with sucessfully!\n");

	disconnectSrvPainel();

	return(0);
}
