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

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define SZ_SG_SQLCMD			(2000)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static char SG_checkLogin_NOROW = SQL_NO_ROW;
static log_t *log = NULL;


/* *** FUNCTIONS *********************************************************************** */
void getLogSystem(log_t *logClient)
{
	log = logClient;
	return;
}

int SG_checkLogin_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	SG_checkLogin_NOROW = SQL_HAS_ROW;
	return(0);
}

int SG_checkLogin(char *user, char *passhash, char *func)
{
	char sql[SZ_SG_SQLCMD + 1] = {'\0'};

	snprintf(sql, SZ_SG_SQLCMD, "SELECT ID FROM %s WHERE ID = '%s' AND FUNCAO = '%s' AND PASSHASH = '%s'", DB_USERS_TABLE, user, func, passhash);

	SG_checkLogin_NOROW = SQL_NO_ROW;

	if(dbSelect(sql, SG_checkLogin_callback, NULL) == NOK){
		logWrite(log, LOGOPALERT, "aaaaaaaaaaaaaaaa [%s]\n", sql); /* TODO: melhorar mesagem */
		return(NOK);
	}

	if(SG_checkLogin_NOROW == SQL_NO_ROW)
		return(NOK);

	return(OK);
}

int SG_fillInDataInsertLogin(char *user, char *func, char *dateTime, char *ip, int port, SG_registroDB_t *data)
{
	/* DRT */
	strcpy(data->drt, user);

	/* DATAHORA */
	strcpy(data->data, dateTime);

	/* FUNCAO */
	strcpy(data->funcao, func);

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

	logWrite(log, LOGDEV, "Inserindo registro:\n"
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

int SG_db_inserting(SG_registroDB_t *data)
{
	char sqlCmd[sizeof(SG_registroDB_t) + 300] = {'\0'};

	memset(sqlCmd, '\0', sizeof(SG_registroDB_t) + 300);

	snprintf(sqlCmd,
	         SQL_COMMAND_SZ,
	         "INSERT INTO %s (DRT,DATAHORA,LOGINOUT,FUNCAO,PANELA,WS,FORNELETR,NUMMAQUINA,DIAMETRO,CLASSE,TEMP,PERCFESI,PERCMG,PERCC,PERCS,PERCP,PERCINOCLNT,ENELETTON,CADENCIA,OEE,ASPECTUBO,REFUGO,IPPORT) " \
	         "VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", DB_MSGS_TABLE,
	         data->drt, data->data, data->loginout, data->funcao, data->panela, data->ws, data->fornEletr, data->numMaquina, data->diamNom, data->classe, data->temp, data->percFeSi,
	         data->percMg, data->percC, data->percS, data->percP, data->percInoculante, data->enerEletTon, data->cadencia, data->oee, data->aspecto, data->refugo, data->ipport);

	logWrite(log, LOGDEV, "Tentando INSERT: [%s]\n", sqlCmd);

	if(dbInsert(sqlCmd) == NOK){
		logWrite(log, LOGOPALERT, "Erro inserindo registro! [%s]\n", sqlCmd);
		return(OK);
	}

	return(OK);
}
