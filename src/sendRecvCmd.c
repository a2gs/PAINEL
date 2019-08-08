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
	cfgFile_t nccCfg;
	unsigned int cfgLineError = 0;
	char *cfgServerAddress = NULL;
	char *cfgServerPort    = NULL;
	/*
	char *cfgLogFile       = NULL;
	char *cfgLogLevel      = NULL;
	*/
	char *cfgNetKey        = NULL;
	char *cfgIVKey         = NULL;

	/*
	if(argc != 4){
		fprintf(stderr, "[%s %d] Usage:\n%s <IP_ADDRESS> <PORT> <FILE_CMDs>\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}
	*/


	if(argc != 3){
		fprintf(stderr, "[%s %d] Usage:\n%s <CONFIG_FILE> <FILE_CMDs>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "CONFIG_FILE sample variables:\n");
		fprintf(stderr, "\t#PAINEL_SERVER_ADDRESS = 123.123.123.123\n");
		fprintf(stderr, "\tPAINEL_SERVER_ADDRESS = painel.servers\n");
		fprintf(stderr, "\tPAINEL_SERVER_PORT = 9998\n");
		/*
		fprintf(stderr, "\tLOG_FILE = ncclient.log\n");
		fprintf(stderr, "\t#Log levels:\n");
		fprintf(stderr, "\t#REDALERT = Red alert\n");
		fprintf(stderr, "\t#DBALERT = Database alert\n");
		fprintf(stderr, "\t#DBMSG = Database message\n");
		fprintf(stderr, "\t#OPALERT = Operation alert\n");
		fprintf(stderr, "\t#OPMSG = Operation message\n");
		fprintf(stderr, "\t#MSG = Just a message\n");
		fprintf(stderr, "\t#DEV = Developer (DEBUG) message\n");
		fprintf(stderr, "\tLOG_LEVEL = REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		*/
		fprintf(stderr, "\tPAINEL_PASSPHRASE = abcdefghijlmnopqrstuvxz\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(cfgFileLoad(&nccCfg, argv[1], &cfgLineError) == CFGFILE_NOK){
		fprintf(stderr, "Error open/loading (at line: [%d]) configuration file [%s]: [%s].\n", cfgLineError, argv[1], strerror(errno));
		return(-2);
	}

	if(cfgFileOpt(&nccCfg, "PAINEL_SERVER_ADDRESS", &cfgServerAddress) == CFGFILE_NOK){
		fprintf(stderr, "Config with label PAINEL_SERVER_ADDRESS not found into file [%s]! Exit.\n", argv[1]);
		return(-3);
	}

	if(cfgFileOpt(&nccCfg, "PAINEL_SERVER_PORT", &cfgServerPort) == CFGFILE_NOK){
		fprintf(stderr, "Config with label PAINEL_SERVER_PORT not found into file [%s]! Exit.\n", argv[1]);
		return(-4);
	}

/*
	if(cfgFileOpt(&nccCfg, "LOG_FILE", &cfgLogFile) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_FILE not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}

	if(cfgFileOpt(&nccCfg, "LOG_LEVEL", &cfgLogLevel) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_LEVEL not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}
	*/

	if(cfgFileOpt(&nccCfg, "NET_KEY", &cfgNetKey) == CFGFILE_NOK){
		fprintf(stderr, "Config with label NET_KEY not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}

	if(cfgFileOpt(&nccCfg, "IV_KEY", &cfgIVKey) == CFGFILE_NOK){
		fprintf(stderr, "Config with label NET_IV not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}
	strncpy(netcrypt.IV, cfgIVKey, IV_SHA256_LEN);

	if(calcHashedNetKey(cfgNetKey, netcrypt.key) == PAINEL_NOK){
		fprintf(stderr, "Fail to hash netkey! Exit.\n");
		return(-7);
	}

	memset(cfgNetKey, 0, strlen(cfgNetKey));
	memset(cfgIVKey,  0, strlen(cfgIVKey ));
	getLogSystem_Util(NULL);

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	fprintf(stderr, "StartUp Client [%s]! Server: [%s] Port: [%s] Cmd file: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), argv[1], argv[2], argv[3], getPAINELEnvHomeVar());

	if(connectSrvPainel(argv[1], argv[2]) == PAINEL_NOK){
		logWrite(NULL, LOGOPALERT, "Erro conetando ao servidor PAINEL [%s:%s].\n", argv[1], argv[2]);
		return(-8);
	}

	f = fopen(argv[3], "r");
	if(f == NULL){
		printf("Unable to open file: [%s]\n", strerror(errno));
		return(-9);
	}

	for(; fgets(line, MAXLINE, f) != NULL; ){
		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		fprintf(stderr, "Sending: [%s] Bytes: [%ld]\n", line, strlen(line));
		if(sendToNet(getSocket(), line, strlen(line), &srError, NULL) == PAINEL_NOK){
			fprintf(stderr, "sendToNet() error to line [%s]: [%s].\n", line, strerror(srError));
			break;
		}

		if(recvFromNet(getSocket(), line, MAXLINE, &recvSz, &srError, NULL) == PAINEL_NOK){
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
