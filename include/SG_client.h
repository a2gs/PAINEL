/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* SG_client.h
 * <File description>
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __SG_CLIENT_H__
#define __SG_CLIENT_H__


/* *** INCLUDES ****************************************************** */
/* #include "sc.h" */
#include "SG.h"


/* *** DEFINES ******************************************************* */
#define DRT_FILE				("DRTs.text")

#define STR_FORNOELETRICO	("FornoEletrico")
#define STR_OPERMAQUINA		("OperadorMaquina")
#define STR_SUPMAQUINA		("SupervisorMaquina")


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef enum{
	FORNO_ELETRICO = 1,
	OPERADOR_MAQUINA,
	SUPERVISOR_MAQUINA,
	UNDEFINED_USER
}tipoUsuario_t;

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
/* int SG_sendExit(int socket, char *drt, char *funcao)
 *
 * Send logoff message to server.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 *  funcao - User Level (office responsibility)
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error sending (send(2))
 */
int SG_sendExit(int socket, char *drt, char *funcao);

/* int SG_sendLogin(int socket, char *drt, char *passhash, char *funcao)
 *
 * Send login message to server.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 *  passhash - Hashed password
 *  funcao - User Level (office responsibility)
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error sending (send(2))
 */
int SG_sendLogin(int socket, char *drt, char *passhash, char *funcao);

/* int SG_interfaceSupervisorMaquina(char *drt, int socket)
 *
 * SupervisorMaquina screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  OK - Data collected
 *  NOK - Data did not collect
 */
int SG_interfaceSupervisorMaquina(char *drt, int socket);

/* int SG_interfaceOperadorMaquina(char *drt, int socket)
 *
 * OperadorMaquina screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  OK - Data collected
 *  NOK - Data did not collect
 */
int SG_interfaceOperadorMaquina(char *drt, int socket);

/* int SG_interfaceFornoEletrico(char *drt, int socket)
 *
 * FornoEletrico screen.
 *
 * INPUT:
 *  socket - Socket to server
 *  drt - User ID
 * OUTPUT:
 *  OK - Data collected
 *  NOK - Data did not collect
 */
int SG_interfaceFornoEletrico(char *drt, int socket);

/* int geraArqDRTs(void)
 *
 * Create a sample 'User Id' X 'User Level' (office responsibility) local file.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  OK - File created
 *  NOK - Erro creation
 */
int geraArqDRTs(void);

/* int SG_fazerLogin(char *drt, char *passhash, char *funcao, tipoUsuario_t *userType)
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
 *  OK - Ok
 *  NOK - Login error
 */
int SG_fazerLogin(char *drt, char *passhash, char *funcao, tipoUsuario_t *userType);

/* int SG_clientScreen(int sockfd, char *drt, char *funcao, tipoUsuario_t userType)
 *
 * Call the correct screen to active user.
 *
 * INPUT:
 *  sockfd - Socket to server
 *  drt - User id typed
 *  funcao - User Level (office responsibility) loaded from file
 *  userType - User level type
 * OUTPUT:
 *  OK - Ok
 *  NOK - Screen error
 */
int SG_clientScreen(int sockfd, char *drt, char *funcao, tipoUsuario_t userType);

#endif
