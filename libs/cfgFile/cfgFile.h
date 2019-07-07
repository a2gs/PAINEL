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


#ifndef __CFG_FILE_H__
#define __CFG_FILE_H__

/* *** INCLUDES ************************************************************************ */
#include "linkedlist.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define CFGFILE_OK  (0)
#define CFGFILE_NOK (1)

#define CFGFILE_LINE_SZ  (30000)
#define CFGFILE_VALUE_SZ (CFGFILE_LINE_SZ - 200)
#define CFGFILE_LABEL_SZ (CFGFILE_LINE_SZ - CFGFILE_VALUE_SZ)

typedef struct _cfgFile_t{
	ll_node_t *head;
}cfgFile_t;


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */

int cfgFileLoad(cfgFile_t *ctx, char *cfgFilePath, unsigned int *lineError);
int cfgFileOpt(cfgFile_t *ctx, char *label, char **value);
int cfgFileFree(cfgFile_t *ctx);

/* int cfgReadOpt(char *pathCfg, char *opt, char *cfg, size_t cfgSz)
 *
 * Read a file ('configuration file', format like above).
 *
 * INPUT:
 *  pathCfg - Full path to configuration file
 *  opt - Option 'key'
 *  cfgSz - Option buffer size output
 *  
 * OUTPUT:
 *  PAINEL_OK - Open and read cfg file (it will not indicate if the option key was located. Just if the cfg file was able to be read)
 *  PAINEL_NOK - Unable to read cfg file (logged)
 *  cfg - Return the value of 'option key' (with max cfgSz length). Space and TAB are suppressed at begin and tail (alltrim())
 *  cfgSzRead - Value' length read ('\0' at cfg)
 */
/* Cfg file sample (max option key length is OPTLINE_CFG_BUFF_SZ and max line (key+value+'=') is LINE_CFG_BUFF_SZ):
 *
 * #Comment 1
 * opt1	=	abc
 * opt2= def
 * # Comment 2
 *
 * opt3=		ghi
 */
int cfgFileReadValue(char *pathCfg, char *opt, char *cfg, size_t cfgSz, size_t *cfgSzRead);

#endif
