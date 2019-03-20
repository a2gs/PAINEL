/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
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
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};
	int rc = 0;
	char fExcel[SZ_EXCELFILENAME + 1] = {'\0'};
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};

	snprintf(fExcel, SZ_EXCELFILENAME, "%s/%s/%s_Static.html", getPAINELEnvHomeVar(), DATABASE_PATH, EXCEL_FILE);
	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	excel = fopen(fExcel, "w");
	if(excel == NULL){
		fprintf(stderr, "Erro criando Excel [%s]. Motivo: [%s]\n", fExcel, strerror(errno));
		return(1);
	}

	rc = sqlite3_enable_shared_cache(1);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else{
			fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}

		fprintf(stderr, "Cannot enable shared cache database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		return(1);
	}

	/*rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, NULL);*/
	rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else{
			fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}

		fprintf(stderr, "Cannot open database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
    
	snprintf(sql, SQL_COMMAND_SZ, "SELECT * FROM %s", DB_MSGS_TABLE);

	rc = sqlite3_exec(db, sql, excel_constructTable, 0, &err_msg);
    
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else{
			fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}
        
		fprintf(stderr, "Failed to select data\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);
        
		return(1);
	} 
    
	fclose(excel);

	if(sqlite3_close_v2(db) != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}else{
			fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		}

		fprintf(stderr, "SQL close error!\n");
		return(1);
	}

	return(0);
}
