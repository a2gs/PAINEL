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
		logWrite(log, LOGOPALERT, "Error selecting user from table.\n");
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
	char *p = NULL;

	p = msg;

	/* DRT */
	cutter(&p, '|', data->drt, DRT_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui1\n");

	/* DATAHORA */
	cutter(&p, '|', data->data, DATA_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui2\n");

	/* LOGINOUT */
	cutter(&p, '|', data->loginout, 1);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui3\n");

	/* FUNCAO */
	cutter(&p, '|', data->funcao, VALOR_FUNCAO_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui4\n");

	/* PANELA */
	cutter(&p, '|', data->panela, FORNELET_PAMELA_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui5\n");

	/* WS */
	cutter(&p, '|', data->ws, OPEMAQ_WS_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui6\n");

	/* FORNELETR */
	cutter(&p, '|', data->fornEletr, FORNELET_NUMFORELE_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui7\n");

	/* NUMMAQUINA */
	cutter(&p, '|', data->numMaquina, OPEMAQ_NUMMAQ_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui8\n");

	/* DIAMETRO */
	cutter(&p, '|', data->diamNom, OPEMAQ_DIAMNOM_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui9\n");

	/* CLASSE */
	cutter(&p, '|', data->classe, OPEMAQ_CLASSE_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui10\n");

	/* TEMP */
	cutter(&p, '|', data->temp, OPEMAQ_TEMP_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui11\n");

	/* PERCFESI */
	cutter(&p, '|', data->percFeSi, OPEMAQ_PERC_FESI_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui12\n");

	/* PERCMG */
	cutter(&p, '|', data->percMg, FORNELET_PERC_MG_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui13\n");

	/* PERCC */
	cutter(&p, '|', data->percC, FORNELET_PERC_C_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui14\n");

	/* PERCS */
	cutter(&p, '|', data->percS, FORNELET_PERC_S_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui15\n");

	/* PERCP */
	cutter(&p, '|', data->percP, FORNELET_PERC_P_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui16\n");

	/* PERCINOCLNT */
	cutter(&p, '|', data->percInoculante, OPEMAQ_PERC_INOC_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui17\n");

	/* ENELETTON */
	cutter(&p, '|', data->enerEletTon, OPEMAQ_ENEELETON_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui18\n");

	/* CADENCIA */
	cutter(&p, '|', data->cadencia, SUPMAQ_CADENCIA);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui19\n");

	/* OEE */
	cutter(&p, '|', data->oee, SUPMAQ_OEE);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui20\n");

	/* ASPECTUBO */
	cutter(&p, '|', data->aspecto, SUPMAQ_ASPEC_LEN);
	if(*p == '\0') return(NOK);

	logWrite(log, LOGDEV, "aqui21\n");

	/* REFUGO (ultimo) */
	cutter(&p, '|', data->refugo, SUPMAG_REFUGO_LEN);

	logWrite(log, LOGDEV, "aqui22\n");

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
