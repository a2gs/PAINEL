/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* userId.h
 * Functions to manage userId (DRT) to local file DRT_FILE.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 03/05/2018 | Creation
 *          |            |
 */

#ifndef __USERID_H__
#define __USERID_H__


/* *** INCLUDES ****************************************************** */
#include "SG.h"
/*
#include "SG_client.h"
*/

#include <linkedlist.h>


/* *** DEFINES ******************************************************* */
#define DRT_FILE				("DRTs.text")

#define STR_FORNOELETRICO	("FornoEletrico")
#define STR_OPERMAQUINA		("OperadorMaquina")
#define STR_SUPMAQUINA		("SupervisorMaquina")

#define DRT_FULLFILEPATH_SZ      (300)
#define LINE_DRT_FILE_LEN        (200)

#define STR_USR_FIELD_TEXT  ("TEXT")
#define STR_USR_FIELD_NUM   ("NUM")
#define STR_USR_FIELD_DATE  ("DATE")

/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef enum{
	FORNO_ELETRICO = 1,
	OPERADOR_MAQUINA,
	SUPERVISOR_MAQUINA,
	UNDEFINED_USER
}tipoUsuario_t;

typedef struct _userId_t{
	char userId[DRT_LEN + 1];
	tipoUsuario_t level;
}userId_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* int loadUserIdFileToMemory(ll_node **head, char *userIdFullPath)
 *
 * Open an load to a linkedlist (lib) the UserId DRT_FILE (DRTs.text) file.
 *
 * INPUT:
 *  head - Linkedlist header address
 *  userIdFullPath - DTR file full path
 * OUTPUT:
 *  PAINEL_OK - 
 *  PAINEL_NOK - 
 */
int loadUserIdFileToMemory(ll_node_t **head, char *userIdFullPath);

int dumpUserIdMemoryFromFile(ll_node_t **head, char *userIdFullPath);

/* char * userType_t_2_String(tipoUsuario_t usrType)
 *
 * A given tipoUsuario_t, return the human string.
 *
 * INPUT:
 *  usrType - Enum tipoUsuario_t for a user level.
 * OUTPUT:
 *  Pointer to a const string.
 */
char * userType_t_2_String(tipoUsuario_t usrType);

/* tipoUsuario_t string_2_UserType_t(char *userStr);
 *
 *  A given human string level, return the respective tipoUsuario_t.
 *
 * INPUT:
 *  userStr - User level string
 * OUTPUT:
 *  tipoUsuario_t for that given string.
 */
tipoUsuario_t string_2_UserType_t(char *userStr);

#endif
