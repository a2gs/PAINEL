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

#include <cfgFile.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static netpass_t netcrypt = {{'\0'}, {'\0'}};


/* *** FUNCTIONS *********************************************************************** */
/* int main(int argc, char *argv[])
 *
 * INPUT:
 *  argv[1] - Cfg file
 *  argv[2] - Command file
 * OUTPUT (to SO):
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	FILE *f = NULL;
	char line[MAXLINE] = {'\0'};
	char serverAddress[SERVERADDRESS_SZ + 1] = {'\0'};
	char serverPort[SERVERPORT_SZ + 1] = {'\0'};
	char *c = NULL;
	int srError = 0;
	size_t recvSz = 0;
	cfgFile_t srcmdCfg;
	unsigned int cfgLineError = 0;
	char *cfgServerAddress = NULL;
	char *cfgServerPort    = NULL;
	char *cfgNetKey        = NULL;
	char *cfgIVKey         = NULL;

	if(argc != 3){
		printf("[%s %d] Usage:\n%s <CONFIG_FILE> <FILE_CMDs>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		printf("CONFIG_FILE sample variables:\n");
		printf("\t#PAINEL_SERVER_ADDRESS = 123.123.123.123\n");
		printf("\tPAINEL_SERVER_ADDRESS = painel.servers\n");
		printf("\tPAINEL_SERVER_PORT = 9998\n");
		printf("\tNET_KEY = abcdefghijlmnopqrstuvxz\n");
		printf("\t#KEY_IV (128 bits length)\n");
		printf("\tKEY_IV = 0123456789012345\n\n");
		printf("PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(cfgFileLoad(&srcmdCfg, argv[1], &cfgLineError) == CFGFILE_NOK){
		printf("Error open/loading (at line: [%d]) configuration file [%s]: [%s].\n", cfgLineError, argv[1], strerror(errno));
		return(-2);
	}

	if(cfgFileOpt(&srcmdCfg, "PAINEL_SERVER_ADDRESS", &cfgServerAddress) == CFGFILE_NOK){
		printf("Config with label PAINEL_SERVER_ADDRESS not found into file [%s]! Exit.\n", argv[1]);
		return(-3);
	}

	if(cfgFileOpt(&srcmdCfg, "PAINEL_SERVER_PORT", &cfgServerPort) == CFGFILE_NOK){
		printf("Config with label PAINEL_SERVER_PORT not found into file [%s]! Exit.\n", argv[1]);
		return(-4);
	}

	if(cfgFileOpt(&srcmdCfg, "NET_KEY", &cfgNetKey) == CFGFILE_NOK){
		printf("Config with label NET_KEY not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}
	strncpy((char *)netcrypt.key, cfgNetKey, PASS_SHA256_ASCII_LEN);

	if(cfgFileOpt(&srcmdCfg, "NET_IV", &cfgIVKey) == CFGFILE_NOK){
		printf("Config with label NET_IV not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}
	strncpy((char *)netcrypt.IV, cfgIVKey, IV_SHA256_LEN);

	/* TODO ..... whyyy???
	if(calcHashedNetKey(cfgNetKey, netcrypt.key) == PAINEL_NOK){
		printf("Fail to hash netkey! Exit.\n");
		return(-7);
	}
	*/

	memset(cfgNetKey, 0, strlen(cfgNetKey));
	memset(cfgIVKey,  0, strlen(cfgIVKey ));

	strncpy(serverAddress, cfgServerAddress, SERVERADDRESS_SZ);
	strncpy(serverPort,    cfgServerPort,    SERVERPORT_SZ   );

	getLogSystem_Util(NULL);

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	printf("StartUp Client [%s]! Server: [%s] Port: [%s] Cmd file: [%s] Cfg file: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), serverAddress, serverPort, argv[2], argv[1], getPAINELEnvHomeVar());

	if(connectSrvPainel(serverAddress, serverPort) == PAINEL_NOK){
		logWrite(NULL, LOGOPALERT, "Erro conetando ao servidor PAINEL [%s:%s].\n", serverAddress, serverPort);
		return(-8);
	}

	f = fopen(argv[2], "r");
	if(f == NULL){
		printf("Unable to open file [%s]: [%s]\n", argv[2], strerror(errno));
		return(-9);
	}

	if(cfgFileFree(&srcmdCfg) == CFGFILE_NOK){
		printf("Error at cfgFileFree().\n");
		return(-10);
	}

	for(; fgets(line, MAXLINE, f) != NULL; ){
		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		printf("Sending: [%s] Bytes: [%ld]\n", line, strlen(line));
		if(sendToNet(getSocket(), line, strlen(line), &srError, &netcrypt) == PAINEL_NOK){
			printf("sendToNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}

		if(recvFromNet(getSocket(), line, MAXLINE, &recvSz, &srError, &netcrypt) == PAINEL_NOK){
			printf("recvFromNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}
		printf("Received: [%.*s] Bytes: [%ld].\n\n", (int)recvSz, line, recvSz);

	}

	fclose(f);

	printf("Terminating application with sucessfully!\n");

	disconnectSrvPainel();

	return(0);
}
