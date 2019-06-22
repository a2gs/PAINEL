/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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
typedef struct _getUserIFace_t{
	char *buf;
	size_t bufSz;
	size_t bufSzUsed;
}getUserIFace_t;


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static char SG_NOROW_RET = SQL_NO_ROW;
static log_t *log = NULL;


/* *** FUNCTIONS *********************************************************************** */
void getLogSystem_SGServer(log_t *logClient)
{
	log = logClient;
	return;
}

int SG_checkLogin_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	SG_NOROW_RET = SQL_HAS_ROW;
	return(0);
}

int SG_checkLogin(char *user, char *passhash, char *func)
{
	char sql[SZ_SG_SQLCMD + 1] = {'\0'};

	snprintf(sql, SZ_SG_SQLCMD, "SELECT ID FROM %s WHERE ID = '%s' AND FUNCAO = '%s' AND PASSHASH = '%s'", DB_USERS_TABLE, user, func, passhash);

	SG_NOROW_RET = SQL_NO_ROW;

	if(dbSelect(sql, SG_checkLogin_callback, NULL) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "Error selecting user login from table.\n");
		return(PAINEL_NOK);
	}

	if(SG_NOROW_RET == SQL_NO_ROW)
		return(PAINEL_NOK);

	return(PAINEL_OK);
}

int SG_fillInDataInsertLogout(char *user, char *func, char *dateTime, char *ip, int port, SG_registroDB_t *data)
{
	/* DRT */
	strcpy(data->drt, user);

	/* DATAHORA */
	strcpy(data->data, dateTime);

	/* FUNCAO */
	strcpy(data->funcao, func);

	/* LOGINOUT */
	strncpy(data->loginout, "O", 1);

	/* CLIENT IP/PORT */
	snprintf(data->ipport, VALOR_IPPORT_LEN, "%s:%d", ip, port);

	return(PAINEL_OK);
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

	return(PAINEL_OK);
}

int protocolChecking(char *msg)
{
	register int n = 0;
	register char *p = NULL;

	for(p = msg, n = 0; *p != '\0'; p++)
		if(*p == '|') n++;

	if(n != 21)
		return(PAINEL_NOK);

	return(PAINEL_OK);
}

inline int protocolChecking(char *msg);

int SG_parsingDataInsertRegistro(char *msg, char *ip, int port, SG_registroDB_t *data)
{
	char *p = NULL;

	p = msg;

	if(protocolChecking(p) == PAINEL_NOK){
		logWrite(log, LOGDEV, "There arent the correct number (21) of delimitators!\n");
		return(PAINEL_NOK);
	}

	/* DRT */
	cutter(&p, '|', data->drt, DRT_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* DATAHORA */
	cutter(&p, '|', data->data, DATA_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* LOGINOUT */
	cutter(&p, '|', data->loginout, 1);
	if(*p == '\0') return(PAINEL_NOK);

	/* FUNCAO */
	cutter(&p, '|', data->funcao, VALOR_FUNCAO_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PANELA */
	cutter(&p, '|', data->panela, FORNELET_PAMELA_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* WS */
	cutter(&p, '|', data->ws, OPEMAQ_WS_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* FORNELETR */
	cutter(&p, '|', data->fornEletr, FORNELET_NUMFORELE_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* NUMMAQUINA */
	cutter(&p, '|', data->numMaquina, OPEMAQ_NUMMAQ_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* DIAMETRO */
	cutter(&p, '|', data->diamNom, OPEMAQ_DIAMNOM_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* CLASSE */
	cutter(&p, '|', data->classe, OPEMAQ_CLASSE_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* TEMP */
	cutter(&p, '|', data->temp, OPEMAQ_TEMP_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCFESI */
	cutter(&p, '|', data->percFeSi, OPEMAQ_PERC_FESI_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCMG */
	cutter(&p, '|', data->percMg, FORNELET_PERC_MG_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCC */
	cutter(&p, '|', data->percC, FORNELET_PERC_C_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCS */
	cutter(&p, '|', data->percS, FORNELET_PERC_S_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCP */
	cutter(&p, '|', data->percP, FORNELET_PERC_P_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* PERCINOCLNT */
	cutter(&p, '|', data->percInoculante, OPEMAQ_PERC_INOC_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* ENELETTON */
	cutter(&p, '|', data->enerEletTon, OPEMAQ_ENEELETON_LEN);
	if(*p == '\0') return(PAINEL_NOK);

	/* CADENCIA */
	cutter(&p, '|', data->cadencia, SUPMAQ_CADENCIA);
	if(*p == '\0') return(PAINEL_NOK);

	/* OEE */
	cutter(&p, '|', data->oee, SUPMAQ_OEE);
	if(*p == '\0') return(PAINEL_NOK);

	/* ASPECTUBO */
	cutter(&p, '|', data->aspecto, SUPMAQ_ASPEC_LEN);

	/* REFUGO (ultimo) */
	cutter(&p, '|', data->refugo, SUPMAG_REFUGO_LEN);

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

	return(PAINEL_OK);
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

	if(dbInsert(sqlCmd) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "Erro inserindo registro! [%s]\n", sqlCmd);
		return(PAINEL_NOK);
	}

	return(PAINEL_OK);
}

int SG_GetUserIFace_callback(void *dt, int argc, char **argv, char **azColName)
{
	int i = 0;
	getUserIFace_t *data = NULL;

	SG_NOROW_RET = SQL_HAS_ROW;
	data = dt;

	if(data->bufSzUsed != 0){
		data->buf[data->bufSzUsed++] = '|';
		data->buf[data->bufSzUsed]   = '\0';
	}

/* logWrite(log, LOGOPALERT, "aqui 111 [%s][%d][%d]\n", data->buf, data->bufSzUsed, data->bufSz); */

	for(i = 0; i < argc; i++){

/* logWrite(log, LOGOPALERT, "aqui 2222 [%s][%d]", argv[i], i); */

		if(data->bufSzUsed >= data->bufSz)
			return(0); /* error: small buffer */

		data->bufSzUsed += n_strncpy(&data->buf[data->bufSzUsed], argv[i], data->bufSz - data->bufSzUsed);

/* logWrite(log, LOGOPALERT, " ----- [%s]\n", data->buf); */

		if(i < argc - 1){
			data->buf[data->bufSzUsed++] = ':';
			data->buf[data->bufSzUsed]   = '\0';
		}

	}

/* logWrite(log, LOGOPALERT, "aqui 333 [%s][%d][%d]\n", data->buf, data->bufSzUsed, data->bufSz); */

	return(0);
}

int SG_getUserIFace(char *msgBackToClient, size_t msgBackToClientSz, char *usrLevel)
{
	char sqlCmd[SZ_SG_SQLCMD + 1] = {'\0'};
	getUserIFace_t data;

	memset(sqlCmd, '\0', SZ_SG_SQLCMD);
	data.buf       = msgBackToClient;
	data.bufSz     = msgBackToClientSz;
	data.bufSzUsed = 0;

	snprintf(sqlCmd,
	         SQL_COMMAND_SZ,
				"SELECT FIELD, FTYPE, FFMT, FDESC FROM %s WHERE FUNCAO = '%s' ORDER BY FORDER ASC",
	         DB_USRIFACE_TABLE, usrLevel);

	SG_NOROW_RET = SQL_NO_ROW;

/* logWrite(log, LOGOPALERT, "aqui 11\n"); */

	if(dbSelect(sqlCmd, SG_GetUserIFace_callback, &data) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "Error selecting user iface from table.\n");
		return(PAINEL_NOK);
	}

/* logWrite(log, LOGOPALERT, "aqui 22\n"); */

	if(SG_NOROW_RET == SQL_NO_ROW)
		return(PAINEL_NOK);

	return(PAINEL_OK);
}

int SG_GetLevels_callback(void *dt, int argc, char **argv, char **azColName)
{
	char *data = NULL;
#define SG_SERV_DB_LEVEL_LINE   (300)
	char *fmtMask = NULL;
	char fmt[SG_SERV_DB_LEVEL_LINE + 1] = {'\0'};

	SG_NOROW_RET = SQL_HAS_ROW;
	data = dt;

	memset(fmt, 0, SG_SERV_DB_LEVEL_LINE + 1);

	if(data[0] == '\0') /* first element */
		fmtMask = "%s:%s";
	else
		fmtMask = "|%s:%s";

	snprintf(fmt, SG_SERV_DB_LEVEL_LINE, fmtMask, argv[0], argv[1]);

	strcat(data, fmt); /* TODO: strncat() */

	return(0);
}

int SG_getLevels(char *msgBackToClient, size_t msgBackToClientSz)
{
	char sqlCmd[SZ_SG_SQLCMD + 1] = {'\0'};

	memset(sqlCmd, '\0', SZ_SG_SQLCMD);

	snprintf(sqlCmd,
	         SQL_COMMAND_SZ,
				"SELECT FUNCAO, DESCRIPT FROM %s WHERE FUNCAO IS NOT 'All'",
	         DB_USERLEVEL_TABLE);

	SG_NOROW_RET = SQL_NO_ROW;

	if(dbSelect(sqlCmd, SG_GetLevels_callback, msgBackToClient) == PAINEL_NOK){
		logWrite(log, LOGOPALERT, "Error selecting levels from table.\n");
		return(PAINEL_NOK);
	}

	if(SG_NOROW_RET == SQL_NO_ROW)
		return(PAINEL_NOK);

	return(PAINEL_OK);
}
