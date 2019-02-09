/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
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
#include <sqlite3.h>

#include "util.h"
#include "db.h"


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
	int rc = 0;
	char *sql = NULL;
	char *err_msg = NULL;
	sqlite3 *SG_db = NULL;
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};

	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);
	fprintf(stderr, "Criando banco: [%s]\n", DBPath);

	rc = sqlite3_open_v2(DBPath, &SG_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);

	if(rc != SQLITE_OK){
		fprintf(stderr, "Cannot open database [%s]: [%s]\n", DBPath, sqlite3_errmsg(SG_db));
		sqlite3_close(SG_db);

		return(NOK);
	}

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

	sql = "CREATE TABLE IF NOT EXISTS MSGS ("
	                                        "DRT         TEXT NOT NULL, "
	                                        "DATAHORA    TEXT NOT NULL, "
	                                        "LOGINOUT    TEXT, "
	                                        "FUNCAO      TEXT, "
	                                        "PANELA      TEXT, "
	                                        "WS          TEXT, "
	                                        "FORNELETR   TEXT, "
	                                        "NUMMAQUINA  TEXT, "
	                                        "DIAMETRO    TEXT, "
	                                        "CLASSE      TEXT, "
	                                        "TEMP        TEXT, "
	                                        "PERCFESI    TEXT, "
	                                        "PERCMG      TEXT, "
	                                        "PERCC       TEXT, "
	                                        "PERCS       TEXT, "
	                                        "PERCP       TEXT, "
	                                        "PERCINOCLNT TEXT, "
	                                        "ENELETTON   TEXT, "
	                                        "CADENCIA    TEXT, "
	                                        "OEE         TEXT, "
	                                        "ASPECTUBO   TEXT, "
	                                        "REFUGO      TEXT, "
	                                        "IPPORT      TEXT, "
	                                        "PRIMARY KEY(DATAHORA, DRT)"
	                                        ")";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create MSGS error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	sql = "CREATE INDEX IF NOT EXISTS FUNCAO_INDX ON MSGS (FUNCAO)";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index FUNCAO_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	sql = "CREATE INDEX IF NOT EXISTS DATAHORA_INDX ON MSGS (DATAHORA)";
	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

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

	sql = "CREATE TABLE IF NOT EXISTS USERS ("
	                                        "ID          TEXT NOT NULL, "
	                                        "FUNCAO      TEXT, "
	                                        "PASSHASH    TEXT, "
	                                        "PRIMARY KEY(ID)"
	                                        ")";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create USERS error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	sql = "CREATE INDEX IF NOT EXISTS ID_INDX ON USERS (ID)";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

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

	sql = "CREATE TABLE IF NOT EXISTS RELATS ("
	                                         "FUNCAO   TEXT, "
	                                         "TITULO   TEXT, "
	                                         "CAMPOS   TEXT, "
	                                         "HEADERS  TEXT, "
	                                         "PRIMARY KEY(FUNCAO)"
	                                         ")";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create RELATS error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	sql = "CREATE INDEX IF NOT EXISTS FUNC_INDX ON RELATS (FUNCAO)";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL create index FUNC_INDX error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	/* ------------------------------------------------------------------------------------- */

	if(sqlite3_close_v2(SG_db) != SQLITE_OK){
		fprintf(stderr, "SQL close error!\n");
		return(NOK);
	}
	fprintf(stderr, "Banco SQL criado!\n");

	return(OK);
}
