/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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
#define LOG_OK  (0)
#define LOG_NOK (1)

#define LOGMUSTLOGIT (unsigned int)0x1
#define LOGREDALERT  (unsigned int)0x2
#define LOGDBALERT   (unsigned int)0x4
#define LOGDBMSG     (unsigned int)0x8
#define LOGOPALERT   (unsigned int)0x10
#define LOGOPMSG     (unsigned int)0x20
#define LOGMSG       (unsigned int)0x40
#define LOGDEV       (unsigned int)0x80

#define LOG_CMDMASK_SZ           (sizeof("|REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV|"))


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _log_t{
	unsigned int level;
	int fd;
}log_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* int logWrite(log_t *log, unsigned int msgLevel, char *msg, ...)
 *
 * <Description>
 *
 * INPUT:
 *  log - log context
 *  msgLevel - Bit OR between: LOGMUSTLOGIT|LOGREDALERT|LOGDBALERT|LOGDBMSG|LOGOPALERT|LOGOPMSG|LOGMSG|LOGDEV
 *  msg - printf() like format string
 *  ... - parameters
 * OUTPUT:
 *  <None>
 */
int logWrite(log_t *log, unsigned int msgLevel, char *msg, ...);

/* int logCreate(log_t *log, char *fullPath, char *cmdLog)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int logCreate(log_t *log, char *fullPath, char *cmdLog);

/* int logClose(log_t *log)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int logClose(log_t *log);

#endif
