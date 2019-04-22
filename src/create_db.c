/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * MIT License
 *
 */


/* create_db.c
 * Just to create SQLite databases (an auxiliary tool).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/types.h>

#include "util.h"
#include "db.h"
#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int main(int argc, char *argv[])
 *
 * creadte_db starts.
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error
 */
int main(int argc, char *argv[])
{
	log_t log;

	if(argc != 3){
		fprintf(stderr, "[%s %d] Usage:\n%s <LOG_FULL_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "Where WWW, XXX, YYY and ZZZ are a combination (surrounded by \"'\" and separated by \"|\") of: REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "\tREDALERT = Red alert\n");
		fprintf(stderr, "\tDBALERT = Database alert\n");
		fprintf(stderr, "\tDBMSG = Database message\n");
		fprintf(stderr, "\tOPALERT = Operation alert\n");
		fprintf(stderr, "\tOPMSG = Operation message\n");
		fprintf(stderr, "\tMSG = Just a message\n");
		fprintf(stderr, "\tDEV = Developer (DEBUG) message\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(logCreate(&log, argv[1], argv[2]) == LOG_NOK){
		fprintf(stderr, "[%s %d] Unable to open/create [%s]! [%s]\n", time_DDMMYYhhmmss(), getpid(), argv[2], strerror(errno));
		return(-2);
	}

	if(dbOpen(NULL, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, &log) == NOK){
		logWrite(&log, LOGREDALERT, "Erro em abrir/criar banco de dados!\n");
		logClose(&log);
		return(-3);
	}

	if(dbCreateAllTables() == NOK){
		logWrite(&log, LOGREDALERT, "ERRO criando banco de dados (tabelas) SQLite!\n");
		logClose(&log);
		return(-4);
	}

	logWrite(&log, LOGREDALERT, "Banco de dados (tabelas) SQLite criado!\n");

	logClose(&log);
	dbClose();

	return(0);
}
