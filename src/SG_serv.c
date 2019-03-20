/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* SG_serv.c
 * Server side business-specific logic/stuff: protocol formatter, database operations, etc.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "SG_serv.h"
#include "util.h"
#include "db.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define SQL_NO_ROW			(0)
#define SQL_HAS_ROW			(!SQL_NO_ROW)
#define SZ_SG_SQLCMD			(2000)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static sqlite3 *SG_db = NULL;
static char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};
static char SG_checkLogin_NOROW = SQL_NO_ROW;


/* *** FUNCTIONS *********************************************************************** */
int SG_checkLogin_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	SG_checkLogin_NOROW = SQL_HAS_ROW;
	return(0);
}

int SG_checkLogin(char *user, char *passhash, char *func)
{
	int rc = 0;
	char *err_msg = NULL;
	char sql[SZ_SG_SQLCMD + 1] = {'\0'};

	if(SG_db == NULL){
		log_write("Database handle didnt define for insert!\n");
		return(NOK);
	}

	snprintf(sql, SZ_SG_SQLCMD, "SELECT ID FROM %s WHERE ID = '%s' AND FUNCAO = '%s' AND PASSHASH = '%s'", DB_USERS_TABLE, user, func, passhash);
	SG_checkLogin_NOROW = SQL_NO_ROW;

	rc = sqlite3_exec(SG_db, sql, SG_checkLogin_callback, 0, &err_msg);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("SQL insert error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	if(SG_checkLogin_NOROW == SQL_NO_ROW)
		return(NOK);

	return(OK);
}

int SG_parsingDataInsertLogin(char *msg, char *ip, int port, SG_registroDB_t *data)
{
	char *c1 = NULL;
	char *c2 = NULL;

	c1 = c2 = msg;

	/* PROTOCOLO DE PROT_COD_LOGIN
	   <COD|> DRT|DATAHORA|FUNCAO|PASSHASH
	 */

	/* DRT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->drt, c1, c2-c1);
	c1 = c2+1;

	/* DATAHORA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->data, c1, c2-c1);
	c1 = c2+1;

	/* FUNCAO */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->funcao, c1, c2-c1);
	c1 = c2+1;

	/* PASSHASH is ignored. Had used to valited the user */

	/* LOGINOUT */
	strncpy(data->loginout, "I", 1);

	/* CLIENT IP/PORT */
	snprintf(data->ipport, VALOR_IPPORT_LEN, "%s:%d", ip, port);

	return(OK);
}

int SG_parsingDataInsertRegistro(char *msg, char *ip, int port, SG_registroDB_t *data)
{
	char *c1 = NULL;
	char *c2 = NULL;

	c1 = c2 = msg;

	/* PROTOCOLO DE PROT_COD_INSREG:
	   <COD|> DRT|DATAHORA|LOGINOUT|FUCAO|PANELA|WS|FORNELETR|NUMMAQUINA|DIAMETRO|CLASSE|TEMP|PERCFESI|PERCMG|PERCC|PERCS|PERCP|PERCINOCLNT|ENELETTON|CADENCIA|OEE|ASPECTUBO|REFUGO
	 */

	/* DRT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->drt, c1, c2-c1);
	c1 = c2+1;

	/* DATAHORA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->data, c1, c2-c1);
	c1 = c2+1;

	/* LOGINOUT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->loginout, c1, c2-c1);
	c1 = c2+1;

	/* FUNCAO */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->funcao, c1, c2-c1);
	c1 = c2+1;

	/* PANELA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->panela, c1, c2-c1);
	c1 = c2+1;

	/* WS */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->ws, c1, c2-c1);
	c1 = c2+1;

	/* FORNELETR */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->fornEletr, c1, c2-c1);
	c1 = c2+1;

	/* NUMMAQUINA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->numMaquina, c1, c2-c1);
	c1 = c2+1;

	/* DIAMETRO */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->diamNom, c1, c2-c1);
	c1 = c2+1;

	/* CLASSE */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->classe, c1, c2-c1);
	c1 = c2+1;

	/* TEMP */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->temp, c1, c2-c1);
	c1 = c2+1;

	/* PERCFESI */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percFeSi, c1, c2-c1);
	c1 = c2+1;

	/* PERCMG */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percMg, c1, c2-c1);
	c1 = c2+1;

	/* PERCC */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percC, c1, c2-c1);
	c1 = c2+1;

	/* PERCS */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percS, c1, c2-c1);
	c1 = c2+1;

	/* PERCP */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percP, c1, c2-c1);
	c1 = c2+1;

	/* PERCINOCLNT */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->percInoculante, c1, c2-c1);
	c1 = c2+1;

	/* ENELETTON */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->enerEletTon, c1, c2-c1);
	c1 = c2+1;

	/* CADENCIA */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->cadencia, c1, c2-c1);
	c1 = c2+1;

	/* OEE */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->oee, c1, c2-c1);
	c1 = c2+1;

	/* ASPECTUBO */
	c2 = strchr(c1, '|');
	if(c2 == NULL) return(NOK);
	strncpy(data->aspecto, c1, c2-c1);
	c1 = c2+1;

	/* REFUGO (ultimo) */
	strcpy(data->refugo, c1);

	/* CLIENT IP/PORT */
	snprintf(data->ipport, VALOR_IPPORT_LEN, "%s:%d", ip, port);

	log_write("Inserindo registro:\n"
	          "\tdrt[%s]\n"
	          "\tdata[%s]\n"
	          "\tloginout[%s]\n"
	          "\tfuncao[%s]\n"
	          "\tpanela[%s]\n"
	          "\tws[%s]\n"
	          "\tfornEletr[%s]\n"
	          "\tnumMaquina[%s]\n"
	          "\tdiamNom[%s]\n"
	          "\tclasse[%s]\n"
	          "\ttemp[%s]\n"
	          "\tpercFeSi[%s]\n"
	          "\tpercMg[%s]\n"
	          "\tpercC[%s]\n"
	          "\tpercS[%s]\n"
	          "\tpercP[%s]\n"
	          "\tpercInoculante[%s]\n"
	          "\tenerEletTon[%s]\n"
	          "\tcadencia[%s]\n"
	          "\toee[%s]\n"
	          "\taspecto[%s]\n"
	          "\trefugo[%s]\n"
	          "\tipport[%s]\n",
	          data->drt,
	          data->data,
	          data->loginout,
	          data->funcao,
	          data->panela,
	          data->ws,
	          data->fornEletr,
	          data->numMaquina,
	          data->diamNom,
	          data->classe,
	          data->temp,
	          data->percFeSi,
	          data->percMg,
	          data->percC,
	          data->percS,
	          data->percP,
	          data->percInoculante,
	          data->enerEletTon,
	          data->cadencia,
	          data->oee,
	          data->aspecto,
	          data->refugo,
	          data->ipport);

	return(OK);
}

/* ---[DATABASE]------------------------------------------------------------------------------------------- */

int SG_db_open_or_create(void)
{
	int rc = 0;
	char *sql = NULL;
	char *err_msg = NULL;

	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);
	SG_db = NULL;

	rc = sqlite3_enable_shared_cache(1);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("Cannot enable shared cache database [%s]: [%s]\n", DBPath, sqlite3_errmsg(SG_db));
		return(NOK);
	}

	/*rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, NULL);*/
	rc = sqlite3_open_v2(DBPath, &SG_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("Cannot open database [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		sqlite3_close(SG_db);

		return(NOK);
	}

	/*sql = "CREATE TABLE IF NOT EXISTS MSGS (DATE TEXT NOT NULL, DRT TEXT NOT NULL, MSG TEXT, IPPORT TEXT, PRIMARY KEY(DATE, DRT))";*/
	/* DATABASE SCHEMA (tamanhos medios esperados. Todos os dados sao textos):

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
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("SQL create error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	sql = "CREATE INDEX IF NOT EXISTS FUNCAO_INDX ON MSGS (FUNCAO)";

	rc = sqlite3_exec(SG_db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("SQL create index error [%s]: [%s].\n", sql, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	return(OK);
}

int SG_db_inserting(SG_registroDB_t *data)
{
	char sqlCmd[sizeof(SG_registroDB_t) + 300] = {'\0'};
	char *err_msg = NULL;
	int rc = 0;

	if(SG_db == NULL){
		log_write("Database handle didnt define for insert!\n");
		return(NOK);
	}

	memset(sqlCmd, '\0', sizeof(SG_registroDB_t) + 300);

	snprintf(sqlCmd,
	         sizeof(sqlCmd),
	         "INSERT INTO MSGS(DRT,DATAHORA,LOGINOUT,FUNCAO,PANELA,WS,FORNELETR,NUMMAQUINA,DIAMETRO,CLASSE,TEMP,PERCFESI,PERCMG,PERCC,PERCS,PERCP,PERCINOCLNT,ENELETTON,CADENCIA,OEE,ASPECTUBO,REFUGO,IPPORT) "
	         "VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
	         data->drt, data->data, data->loginout, data->funcao, data->panela, data->ws, data->fornEletr, data->numMaquina, data->diamNom, data->classe, data->temp, data->percFeSi,
	         data->percMg, data->percC, data->percS, data->percP, data->percInoculante, data->enerEletTon, data->cadencia, data->oee, data->aspecto, data->refugo, data->ipport);

	/* log_write("Tentando INSERT: [%s]\n", sqlCmd); */

	rc = sqlite3_exec(SG_db, sqlCmd, 0, 0, &err_msg);

	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("SQL insert error [%s]: [%s].\n", sqlCmd, err_msg);
		sqlite3_free(err_msg);

		return(NOK);
	}

	return(OK);
}

int SG_db_close(void)
{
	int rc = 0;

	if(SG_db == NULL){
		log_write("Database handle didnt define for close!\n");
		return(NOK);
	}

	rc = sqlite3_close_v2(SG_db);
	if(rc != SQLITE_OK){
		if(rc == SQLITE_BUSY){
			log_write("SQLITE_BUSY [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED){
			log_write("SQLITE_LOCKED [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
			log_write("SQLITE_LOCKED_SHAREDCACHE [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}else{
			log_write("Another error [%s]: [%s].\n", DBPath, sqlite3_errmsg(SG_db));
		}

		log_write("SQL close error!\n");
		return(NOK);
	}

	memset(DBPath, 0, sizeof(DBPath));

	return(OK);
}
