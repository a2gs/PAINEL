/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* util_network.h
 * Project's auxiliary network functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 04/07/2018 | Creation
 *          |            |
 */


#ifndef __UTIL_NETWORK_H__
#define __UTIL_NETWORK_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */


/* *** INTERFACES / PROTOTYPES *************************************** */

/* int isConnect(void)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int isConnect(void);

/* int getSocket(void)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int getSocket(void);

/* int disconnectSrvPainel(void)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int disconnectSrvPainel(void);

/* int connectSrvPainel(char *srvAdd, char *srvPort, int *log)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int connectSrvPainel(char *srvAdd, char *srvPort, int *log);

#endif
