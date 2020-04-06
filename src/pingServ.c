/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* pingServ.c
 * A ping client to PAINEL serv.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 28/04/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "util.h"
#include "util_network.h"

#include <cfgFile.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */

/* -------------------------------------------------------------------------------------------------------- */

/* int main(int argc, char *argv[])
 *
 * Ping utility.
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
	netpass_t netcrypt = {{'\0'}, {'\0'}};

	unsigned int cfgLineError = 0;
	char *cfgServerAddress = NULL;
	char *cfgServerPort    = NULL;
	char *cfgNetKey        = NULL;
	char *cfgIVKey         = NULL;
	cfgFile_t nccCfg;

	if(argc != 2){
		fprintf(stderr, "[%s] Usage:\n%s <CONFIG_FILE>\n\n", time_DDMMYYhhmmss(), argv[0]);
		fprintf(stderr, "CONFIG_FILE sample variables:\n");
		fprintf(stderr, "\t#PAINEL_SERVER_ADDRESS = 123.123.123.123\n");
		fprintf(stderr, "\tPAINEL_SERVER_ADDRESS = painel.servers\n");
		fprintf(stderr, "\tPAINEL_SERVER_PORT = 9998\n");
		fprintf(stderr, "\tNET_KEY = abcdefghijlmnopqrstuvxz\n");
		fprintf(stderr, "\t#KEY_IV (128 bits length)\n");
		fprintf(stderr, "\tKEY_IV = 0123456789012345\n\n");
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

	if(cfgFileOpt(&nccCfg, "NET_KEY", &cfgNetKey) == CFGFILE_NOK){
		fprintf(stderr, "Config with label NET_KEY not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}
	strncpy((char *)netcrypt.key, cfgNetKey, PASS_SHA256_ASCII_LEN);

	if(cfgFileOpt(&nccCfg, "NET_IV", &cfgIVKey) == CFGFILE_NOK){
		fprintf(stderr, "Config with label NET_IV not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}
	strncpy((char *)netcrypt.IV, cfgIVKey, IV_SHA256_LEN);

	getLogSystem_UtilNetwork(NULL);

	fprintf(stderr, "[%s] Ping [%s] Port: [%s]\n", time_DDMMYYhhmmss(), argv[1], argv[2]);

	if(pingServer(argv[1], argv[2], &netcrypt) == PAINEL_NOK){
		fprintf(stderr, "[%s] Ping erro!\n", time_DDMMYYhhmmss());
		return(-7);
	}

	return(0);
}
