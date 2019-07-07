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

	if(argc != 2){
		printf("Sample usage: %s <PATH_TO_CFG_FILE>\n", argv[0]);
		return(-1);
	}

	if(cfgFileLoad(&ctx, "./sample.cfg", &lineError) == CFGFILE_NOK){
	}

	if(cfgFileOpt(&ctx, "opt1", &value) == CFGFILE_NOK){
	}

	if(cfgFileFree(&ctx) == CFGFILE_NOK){
	}

	return(0);
}
