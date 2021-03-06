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
#include <log.h>


/* *** DEFINES ******************************************************* */
#define SERVERADDRESS_SZ                     (100)
#define SERVERPORT_SZ                        (7)


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _netpass_t{
	unsigned char key[PASS_SHA256_ASCII_LEN + 1];
	unsigned char IV[IV_SHA256_LEN          + 1];
}netpass_t;


/* *** INTERFACES / PROTOTYPES *************************************** */

/* void getLogSystem_UtilNetwork(log_t *lg)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
void getLogSystem_UtilNetwork(log_t *lg);

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

/* int connectSrvPainel(char *srvAdd, char *srvPort)
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int connectSrvPainel(char *srvAdd, char *srvPort);

/* int pingServer(char *ip, char *port, netpass_t *netcrypt)
 *
 *
 *
 * INPUT:
 * OUTPUT:
 *  PAINEL_OK - 
 *  PAINEL_NOK - 
 */
int pingServer(char *ip, char *port, netpass_t *netcrypt);

/* int sendToNet(int sockfd, char *msg, size_t msgSz, int *recvError, netpass_t *netctx)
 *
 * PAINEL Oficial network send. It adds 4 bytes (binary) in front of msg.
 *
 * INPUT:
 *  sockfd - 
 *  msg - 
 *  msgSz - 
 * OUTPUT:
 *  sendError - send(2) errno
 *  PAINEL_OK - 
 *  PAINEL_NOK - 
 */
int sendToNet(int sockfd, char *msg, size_t msgSz, int *sendError, netpass_t *netctx);

/* int recvFromNet(int sockfd, char *msg, size_t msgSz, size_t *recvSz, int *recvError, netpass_t *netctx)
 *
 * PAINEL Oficial network receive. It reads 4 bytes (binary) in front of msg.
 *
 * INPUT:
 *  sockfd - 
 *  msg - 
 *  msgSz - 
 * OUTPUT:
 *  recvSz - 
 *  recvError - recv(2) errno
 *  PAINEL_OK - 
 *  PAINEL_NOK - 
 */
int recvFromNet(int sockfd, char *msg, size_t msgSz, size_t *recvSz, int *recvError, netpass_t *netctx);

#endif
