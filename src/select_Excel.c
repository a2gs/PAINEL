/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* select_Excel.c
 * Creates a Excel file with all database data.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#include "db.h"
#include "util.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define EXCEL_FILE			("database.xls")
#define SZ_EXCELFILENAME	(200)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
FILE *excel = NULL;


/* *** FUNCTIONS *********************************************************************** */
/* int excel_constructTable(void *NotUsed, int argc, char **argv, char **azColName)
 *
 * Constructs the Excel XLS (TAB) file: SQLite callback.
 *
 * INPUT:
 *  NotUsed - not used
 *  argc - number of columns
 *  argv - cell content
 *  azColName - columns name
 * OUTPUT:
 *  0
 */
int excel_constructTable(void *NotUsed, int argc, char **argv, char **azColName)
{
	static int header = 0;
	int i = 0;

	NotUsed = 0;

	if(header == 0){
		for(i = 0; i < argc; i++)
			fprintf(excel, "%s\t", azColName[i]);

		fprintf(excel, "\n");
		header = 1;
	}

	for(i = 0; i < argc; i++)
		fprintf(excel, "%s\t", argv[i] ? argv[i] : "\0");

	fprintf(excel, "\n");

	fflush(excel);
    
	return(0);
}

/* int main(int argc, char *argv[])
 *
 * select_Excel starts.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};
	char fExcel[SZ_EXCELFILENAME + 1] = {'\0'};
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};

	snprintf(fExcel, SZ_EXCELFILENAME, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, EXCEL_FILE);
	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	getLogSystem_Util(NULL); /* Loading log to util functions */
	getLogSystem_DB(NULL); /* Loading log to DB functions */

	if(argc != 1){
		printf("Usage error:\n\t%s\nThe excel dump database will be created at [%s].\nThe database used: [%s].\n", argv[0], fExcel, DBPath);
		return(-1);
	}

	excel = fopen(fExcel, "w+");
	if(excel == NULL){
		printf("Erro criando Excel [%s]. Motivo: [%s]\n", fExcel, strerror(errno));
		return(-2);
	}

	if(dbOpen(NULL, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE/*, NULL*/) == PAINEL_NOK){
		printf("Erro em abrir banco de dados!\n");
		printf("Terminating application!\n");

		dbClose();
		return(-3);
	}

	snprintf(sql, SQL_COMMAND_SZ, "SELECT * FROM %s", DB_MSGS_TABLE);

	if(dbSelect(sql, excel_constructTable, NULL) == PAINEL_NOK){
		printf("Error at select database to define reports [%s]!\n", sql);
		printf("Terminating application!\n");

		dbClose();

		return(-4);
	}

	fclose(excel);
	dbClose();

	return(0);
}
