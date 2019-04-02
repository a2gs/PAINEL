/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* SG_serv.h
 * <File description>
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __SG_SERV_H__
#define __SG_SERV_H__


/* *** INCLUDES ****************************************************** */
#include "util.h"
#include "SG.h"

#include "log.h"


/* *** DEFINES ******************************************************* */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
/* DATABASE SCHEMA */
typedef struct _SG_registroDB_t{
	char drt[DRT_LEN                         + 1];
	char data[DATA_LEN                       + 1];
	char loginout[1                          + 1];
	char funcao[VALOR_FUNCAO_LEN             + 1];
	char panela[FORNELET_PAMELA_LEN          + 1];
	char ws[OPEMAQ_WS_LEN                    + 1];
	char fornEletr[FORNELET_NUMFORELE_LEN    + 1];
	char numMaquina[OPEMAQ_NUMMAQ_LEN        + 1];
	char diamNom[OPEMAQ_DIAMNOM_LEN          + 1];
	char classe[OPEMAQ_CLASSE_LEN            + 1];
	char temp[OPEMAQ_TEMP_LEN                + 1];
	char percFeSi[OPEMAQ_PERC_FESI_LEN       + 1];
	char percMg[FORNELET_PERC_MG_LEN         + 1];
	char percC[FORNELET_PERC_C_LEN           + 1];
	char percS[FORNELET_PERC_S_LEN           + 1];
	char percP[FORNELET_PERC_P_LEN           + 1];
	char percInoculante[OPEMAQ_PERC_INOC_LEN + 1];
	char enerEletTon[OPEMAQ_ENEELETON_LEN    + 1];
	char cadencia[SUPMAQ_CADENCIA            + 1];
	char oee[SUPMAQ_OEE                      + 1];
	char aspecto[SUPMAQ_ASPEC_LEN            + 1];
	char refugo[SUPMAG_REFUGO_LEN            + 1];
	char ipport[VALOR_IPPORT_LEN             + 1];
}SG_registroDB_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* void getLogSystem(log_t *logClient)
 *
 * Save the instance to log system for user interface;
 *
 * INPUT:
 *  logServ - Log opened by client engine.
 * OUTPUT:
 *  none
 */
void getLogSystem(log_t *logServ);

/* int SG_parsingDataInsertRegistro(char *msg, char *ip, int port, SG_registroDB_t *data)
 *
 * Parses the message's data.
 *
 * INPUT:
 *  msg - Message (protocol) from client
 *  ip - Client IP
 *  port - Client PORT
 * OUTPUT:
 *  data - Message's data breakdown
 *  OK - Ok
 *  NOK - Parsing error
 */
int SG_parsingDataInsertRegistro(char *msg, char *ip, int port, SG_registroDB_t *data);

/* int SG_fillInDataInsertLogin(char *user, char *func, char *dateTime, char *ip, int port, SG_registroDB_t *data)
 *
 * Parses the login's message data.
 *
 * INPUT:
 *  user - UserId (DRT)
 *  func - User Level (office responsibility)
 *  dateTime - Login date and time
 *  ip - Client IP
 *  port - Client PORT
 * OUTPUT:
 *  data - Message's data breakdown
 *  OK - Ok
 *  NOK - Parsing error
 */
int SG_fillInDataInsertLogin(char *user, char *func, char *dateTime, char *ip, int port, SG_registroDB_t *data);

/* int SG_checkLogin(char *user, char *passhash, char *func)
 *
 * Check a User ID, User Level and password (hashed).
 *
 * INPUT:
 *  user - User ID (DRT)
 *  passhash - Hashed password
 *  func - User Level (office responsibility)
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error (user does not exist)
 */
int SG_checkLogin(char *user, char *passhash, char *func);

/* int SG_db_open_or_create(void)
 *
 * Create database and its objects: tables, index, etc.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error opening or creating database
 */
/*
int SG_db_open_or_create(void);
*/

/* int SG_db_inserting(SG_registroDB_t *data)
 *
 * Insert a row into database.
 *
 * INPUT:
 *  data - Data to be inserted
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error inserting
 */
int SG_db_inserting(SG_registroDB_t *data);

#endif
