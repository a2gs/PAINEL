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
/* int loadUserIdFileToMemory(ll_node **head)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int loadUserIdFileToMemory(ll_node_t **head, char *userIdFullPath);
char * userType_t_2_String(tipoUsuario_t usrType);
tipoUsuario_t string_2_UserType_t(char *userStr);

#endif
