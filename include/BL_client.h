/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* BL_client.h
 * <File description>
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __BL_CLIENT_H__
#define __BL_CLIENT_H__


/* *** INCLUDES ****************************************************** */
#include "SG.h"
#include "userId.h"

#include <log.h>


/* *** DEFINES ******************************************************* */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _fornoElet_t{
	const char *perguntas[TOT_PERGUNTAS_FORNO_ELET];
	char percFeSi[FORNELET_PERC_FESI_LEN  + 1];
	char percS[FORNELET_PERC_S_LEN        + 1];
	char percMg[FORNELET_PERC_MG_LEN      + 1];
	char panela[FORNELET_PAMELA_LEN       + 1];
	char temp[FORNELET_TEMP_LEN           + 1];
	char fornEletr[FORNELET_NUMFORELE_LEN + 1];
}fornoElet_t;

typedef struct _operMaquina_t{
	const char *perguntas[TOT_PERGUNTAS_OPE_MAQUINA];
	char percFeSi[OPEMAQ_PERC_FESI_LEN       + 1];
	char percInoculante[OPEMAQ_PERC_INOC_LEN + 1];
	char aspecto[OPEMAQ_ASPEC_LEN            + 1];
	char panela[OPEMAQ_PANELA_LEN            + 1];
	char ws[OPEMAQ_WS_LEN                    + 1];
	char diamNom[OPEMAQ_DIAMNOM_LEN          + 1];
	char classe[OPEMAQ_CLASSE_LEN            + 1];
	char temp[OPEMAQ_TEMP_LEN                + 1];
	char numMaquina[OPEMAQ_NUMMAQ_LEN        + 1];
}operMaquina_t;

typedef struct _supMaquina_t{
	const char *perguntas[TOT_PERGUNTAS_SUP_MAQUINA];
	char aspecto[SUPMAQ_ASPEC_LEN + 1];
	char refugo[SUPMAG_REFUGO_LEN + 1];
	char cadencia[SUPMAQ_CADENCIA + 1];
	char oee[SUPMAQ_OEE           + 1];
}supMaquina_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* void getLogSystem_SGClient(log_t *logClient)
 *
 * Save the instance to log system for user interface;
 *
 * INPUT:
 *  logClient - Log opened by client engine.
 * OUTPUT:
 *  none
 */
void getLogSystem_SGClient(log_t *logClient);

/* int BL_sendLogoutExit(int socket, char *drt, char *funcao)
 *
 * Send logoff message to server.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 *  funcao - User Level (office responsibility)
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error sending (send(2))
 */
int BL_sendLogoutExit(int socket, char *drt, char *funcao);

/* int BL_sendLogin(int socket, char *drt, char *passhash, char *funcao)
 *
 * Send login message to server.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 *  passhash - Hashed password
 *  funcao - User Level (office responsibility)
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error sending (send(2))
 */
int BL_sendLogin(int socket, char *drt, char *passhash, char *funcao);

/* int BL_interfaceSupervisorMaquina(char *drt, int socket)
 *
 * SupervisorMaquina screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  PAINEL_OK - Data collected
 *  PAINEL_NOK - Data did not collect
 */
int BL_interfaceSupervisorMaquina(char *drt, int socket);

/* int BL_interfaceOperadorMaquina(char *drt, int socket)
 *
 * OperadorMaquina screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  PAINEL_OK - Data collected
 *  PAINEL_NOK - Data did not collect
 */
int BL_interfaceOperadorMaquina(char *drt, int socket);

/* int BL_interfaceFornoEletrico(char *drt, int socket)
 *
 * FornoEletrico screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  PAINEL_OK - Data collected
 *  PAINEL_NOK - Data did not collect
 */
int BL_interfaceFornoEletrico(char *drt, int socket);

/* int geraArqDRTs(void)
 *
 * Create a sample 'User Id' X 'User Level' (office responsibility) local file.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  PAINEL_OK - File created
 *  PAINEL_NOK - Erro creation
 */
int geraArqDRTs(void);

/* int BL_fazerLogin(char *drt, char *passhash, char *funcao, tipoUsuario_t *userType)
 *
 * Login screen.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  drt - User id typed
 *  passhash - Password hashed
 *  funcao - User Level (office responsibility) loaded from file
 *  userType - User level type
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Login error
 */
int BL_fazerLogin(char *drt, char *passhash, char *funcao, tipoUsuario_t *userType);

/* int BL_clientScreen(int sockfd, char *drt, char *funcao, tipoUsuario_t userType)
 *
 * Call the correct screen to active user.
 *
 * INPUT:
 *  sockfd - Socket to server
 *  drt - User id typed
 *  funcao - User Level (office responsibility) loaded from file
 *  userType - User level type
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Screen error
 */
int BL_clientScreen(int sockfd, char *drt, char *funcao, tipoUsuario_t userType);

#endif
