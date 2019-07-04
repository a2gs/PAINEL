/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* ncclient_util.c
 * ncurses client interface utilities.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 04/07/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <string.h>

#include "ncclient_util.h"
#include "util.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define STR_USR_FIELD_TEXT  ("TEXT")
#define STR_USR_FIELD_NUM   ("NUM")
#define STR_USR_FIELD_DATE  ("DATE")


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static usrFieldCtrl_t usrIfaceFields;


/* *** FUNCTIONS *********************************************************************** */
int usrIsIfaceFieldsEmpty(void)
{
	return((usrIfaceFields.totFields == 0) ? 0 : 1);
}

void usrIfaceFieldsClean(void)
{
	memset(&usrIfaceFields, 0, sizeof(usrFieldCtrl_t));
	usrIfaceFields.totFields = 0;
}

int usrIfaceFieldAdd(char *ffield, char *ftype, char *ffmt, char *fdesc)
{
	if(usrIfaceFields.totFields >= USR_IFACE_TOTAL_FIELDS)
		return(PAINEL_NOK);

	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].field, ffield, USR_IFACE_FIELD_SZ);
	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].fmt,   ffmt,   USR_IFACE_FMTFIELD_SZ);
	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].desc,  fdesc,  USR_IFACE_DESCFIELD_SZ);

	if     (strcmp(ftype, STR_USR_FIELD_TEXT) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = TEXT_USRFIELD;
	else if(strcmp(ftype, STR_USR_FIELD_NUM ) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = NUM_USRFIELD;
	else if(strcmp(ftype, STR_USR_FIELD_DATE) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = DATE_USRFIELD;
	else                                            usrIfaceFields.fields[usrIfaceFields.totFields].type = UNDEFINED_USRFIELD;

	usrIfaceFields.totFields++;

	return(PAINEL_OK);
}
