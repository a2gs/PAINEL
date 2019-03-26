/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* log.h
 * Log system.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __LOG_H__
#define __LOG_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */
#define LOG_MUST_LOG_IT        (unsigned int)0x1
#define LOG_RED_ALERT          (unsigned int)0x2
#define LOG_DATABASE_ALERT     (unsigned int)0x4
#define LOG_DATABASE_MESSAGE   (unsigned int)0x8
#define LOG_OPERATOR_ALERT     (unsigned int)0x10
#define LOG_OPERATOR_MESSAGE   (unsigned int)0x20
#define LOG_MESSAGE            (unsigned int)0x40
#define LOG_DEVELOP            (unsigned int)0x80


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */


/* *** INTERFACES / PROTOTYPES *************************************** */
/* <header function description included by another sources> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
void logPrint(void);

#endif
