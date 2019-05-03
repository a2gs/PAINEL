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
#include <linkedlist.h>


/* *** DEFINES ******************************************************* */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */


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
int loadUserIdFileToMemory(ll_node_t **head);

#endif
