/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* util.c
 * Project's auxiliary functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cfgFile.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
int main(int argc, char *argv[])
{
	cfgFile_t ctx;
	unsigned int lineError = 0;
	char *value = NULL;

	if(argc != 3){
		printf("Sample usage: %s <PATH_TO_CFG_FILE> <LABEL>\n", argv[0]);
		return(-1);
	}

	if(cfgFileLoad(&ctx, "./sample.cfg", &lineError) == CFGFILE_NOK){
		printf("Error open/loading (at line: [%d]) file [%s]: [%s].\n", lineError, argv[1], strerror(errno));
		return(-1);
	}

	if(cfgFileOpt(&ctx, argv[2], &value) == CFGFILE_NOK){
		printf("Label [%s] not found.\n", argv[2]);
		return(-1);
	}

	printf("Label [%s] = [%s].\n", argv[2], value);

	if(cfgFileFree(&ctx) == CFGFILE_NOK){
		printf("Error at cfgFileFree().\n");
		return(-1);
	}

	return(0);
}
