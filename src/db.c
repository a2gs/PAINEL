/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* db.c
 * SQLite databases acesses (an auxiliary tool).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <string.h>
#include <sqlite3.h>

#include "log.h"
#include "util.h"
#include "db.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
static sqlite3 *db = NULL;
static char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};
static char sql[SQL_COMMAND_SZ + 1] = {'\0'};
static log_t *log;


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
int dbInsert(char *sqlCmd)
{
	rc = sqlite3_exec(db, sqlCmd, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			logWrite(log, LOGDBALERT, "SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else{
			logWrite(log, LOGDBALERT, "Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}

		logWrite(log, LOGDBALERT|LOGREDALERT, "SQL insert error [%s]: [%s].\n", sqlCmd, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	return(OK);
}

int dbOpen(char *userDBPath, int flags)
{
	int rc = 0;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};
	char *err_msg = NULL;

	if(userDBPath == NULL)
		snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);
	else
		strncpy(DBPath, userDBPath, DB_PATHFILE_SZ);

	db = NULL;

	rc = sqlite3_enable_shared_cache(1);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			logWrite(log, LOGDBALERT, "SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else{
			logWrite(log, LOGDBALERT, "Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}

		logWrite(log, LOGDBALERT|LOGREDALERT, "Cannot enable shared cache database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		return(NOK);
	}

	/*rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, NULL);*/
	rc = sqlite3_open_v2(DBPath, &db, flags, NULL);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			logWrite(log, LOGDBALERT, "SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else{
			logWrite(log, LOGDBALERT, "Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}

		logWrite(log, LOGDBALERT|LOGREDALERT, "Cannot open database [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		sqlite3_close(db);

		return(NOK);
	}




	return(OK);
}

int createAllPAINELTables(void)
{
	/* ------------------------------------------------------------------------------------- */

	/* DATABASE SCHEMA MSGS (tamanhos medios esperados. Todos os dados sao textos):

	   DRT         25          ALFANUMERICO
	   DATAHORA    DATA        DD/MM/YY HH:MM:SS
      LOGINOUT    1           ALFA         // 'I' IN / 'O' OUT
	   FUNCAO      25          ALFA
	   PANELA      3           NUMERO
	   WS          2,2         NUMERICO     //WETSPRAY
	   FORNELETR   5           ALFA         // FORNO ELETRICO
	   NUMMAQUINA  2           NUMERICO     // NUMERO DA MAQYUINA
	   DIAMETRO    6           NUMERICO     // DN
	   CLASSE      6           ALFANUMERICO // ESPESSURA DO TUBO
	   TEMP        5,2         NUMERICO     // TEMPERATURA
	   PERCFESI    3,2         NUMERICO     // %FERRO-SILICIO
	   PERCMG      3,2         NUMERICO     // % MAGNESIO
	   PERCC       3,3         NUMERICO     // %CARBONO
	   PERCS       3,2         NUMERICO     // %ENXOFRE
	   PERCP       3,3         NUMERICO     // %FOSFORO
	   PERCINOCLNT 3,3         NUMERICO     // %INOCULANTE
	   ENELETTON   5,2         NUMERICO     // Energia Eletrica por TONELADA
	   CADENCIA    3,2         NUMERICO     // CADENCIA
	   OEE         3,2         NUMERICO     // OEE
	   ASPECTUBO   200         ALFA         // ASPECTO DO TUBO
	   REFUGO      200         ALFA         // REFUGO
	   IPPORT      TEXT
	 */

	snprintf(sql, SQL_COMMAND_SZ, "CREATE TABLE IF NOT EXISTS %s (" \
	                              "DRT         TEXT NOT NULL, "     \
	                              "DATAHORA    TEXT NOT NULL, "     \
	                              "LOGINOUT    TEXT, "              \
	                              "FUNCAO      TEXT, "              \
	                              "PANELA      TEXT, "              \
	                              "WS          TEXT, "              \
	                              "FORNELETR   TEXT, "              \
	                              "NUMMAQUINA  TEXT, "              \
	                              "DIAMETRO    TEXT, "              \
	                              "CLASSE      TEXT, "              \
	                              "TEMP        TEXT, "              \
	                              "PERCFESI    TEXT, "              \
	                              "PERCMG      TEXT, "              \
	                              "PERCC       TEXT, "              \
	                              "PERCS       TEXT, "              \
	                              "PERCP       TEXT, "              \
	                              "PERCINOCLNT TEXT, "              \
	                              "ENELETTON   TEXT, "              \
	                              "CADENCIA    TEXT, "              \
	                              "OEE         TEXT, "              \
	                              "ASPECTUBO   TEXT, "              \
	                              "REFUGO      TEXT, "              \
	                              "IPPORT      TEXT, "              \
	                              "PRIMARY KEY(DATAHORA, DRT))",
	         DB_MSGS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create MSGS error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	snprintf(sql, SQL_COMMAND_SZ, "CREATE INDEX IF NOT EXISTS FUNCAO_INDX ON %s (FUNCAO)", DB_MSGS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index FUNCAO_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	snprintf(sql, SQL_COMMAND_SZ, "CREATE INDEX IF NOT EXISTS DATAHORA_INDX ON %s (DATAHORA)", DB_MSGS_TABLE);
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index DATAHORA_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	/* ------------------------------------------------------------------------------------- */

	/* DATABASE SCHEMA USERS (tamanhos medios esperados. Todos os dados sao textos):
	 *
	 * ID (=DRT)				TEXT
	 * LEVEL (=FUNCAO)		TEXT
	 * PASSHASH					TEXT
	 */

	snprintf(sql, SQL_COMMAND_SZ, "CREATE TABLE IF NOT EXISTS %s (" \
	                              "ID TEXT NOT NULL, "              \
	                              "FUNCAO TEXT, "                   \
	                              "PASSHASH TEXT, "                 \
	                              "PRIMARY KEY(ID))",
	         DB_USERS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create [%s] error [%s]: [%s].\n", DB_USERS_TABLE, sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	snprintf(sql, SQL_COMMAND_SZ, "CREATE INDEX IF NOT EXISTS ID_INDX ON %s (ID)", DB_USERS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index ID_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	/* ------------------------------------------------------------------------------------- */

	/* DATABASE SCHEMA RELATORIOS (tamanhos medios esperados. Todos os dados sao textos):
	 *
	 * LEVEL (=FUNCAO)		TEXT
	 * TITULO					TEXT
	 * CAMPOS					TEXT
	 * HEADER (DESCRICOES DOS CAMPOS)           TEXT
	 */

	snprintf(sql, SQL_COMMAND_SZ, "CREATE TABLE IF NOT EXISTS %s (" \
	                              "FUNCAO   TEXT, "                 \
	                              "TITULO   TEXT, "                 \
	                              "CAMPOS   TEXT, "                 \
	                              "HEADERS  TEXT, "                 \
	                              "PRIMARY KEY(FUNCAO))",
	         DB_REPORTS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create RELATS error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	snprintf(sql, SQL_COMMAND_SZ, "CREATE INDEX IF NOT EXISTS FUNC_INDX ON %s (FUNCAO)", DB_REPORTS_TABLE);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index FUNC_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	/* ------------------------------------------------------------------------------------- */

	return(OK);
}

int db_close(void)
{
	int rc = 0;

	if(db == NULL){
		logWrite(log, LOGDBALERT|LOGREDALERT, "Database handle didnt define for close!\n");
		return(NOK);
	}

	rc = sqlite3_close_v2(db);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			logWrite(log, LOGDBALERT, "SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			logWrite(log, LOGDBALERT, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}else{
			logWrite(log, LOGDBALERT, "Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(db));
		}

		logWrite(log, LOGDBALERT|LOGREDALERT, "SQL close error!\n");
		return(NOK);
	}

	memset(DBPath, 0, sizeof(DBPath));

	return(OK);
}

int createAllTables(char *DBPath)
{
	int rc = 0;
	char *err_msg = NULL;
	sqlite3 *db = NULL;
	/*
	rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);

	if(rc != SQLITE_OK){
		fprintf(stderr, "Cannot open database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
		sqlite3_close(db);

		return(NOK);
	}
	*/
	if(db_open(NULL, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE) == NOK){
		logWrite(&log, LOGREDALERT, "Erro em abrir/criar banco de dados!\n");
		logClose(&log);
		return(-4);
	}

	if(createAllPAINELTables() == NOK){
		return(NOK);
	}

	return(OK);
}
