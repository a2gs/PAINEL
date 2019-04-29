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

#include "util.h"


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
	if(argc != 3){
		fprintf(stderr, "[%s] Usage:\n%s <IP_ADDRESS> <PORT>\n", time_DDMMYYhhmmss(), argv[0]);
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	fprintf(stderr, "[%s] Ping [%s] Port: [%s]\n", time_DDMMYYhhmmss(), argv[1], argv[2]);

	if(pingServer(argv[1], argv[2]) == PAINEL_NOK){
		fprintf(stderr, "[%s] Ping erro!\n", time_DDMMYYhhmmss());
		return(-2);
	}

	return(0);
}
