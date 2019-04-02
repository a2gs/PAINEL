/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* db.h
 * Database defines.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __DB_H__
#define __DB_H__


/* *** INCLUDES ****************************************************** */
#include "log.h"


/* *** DEFINES ******************************************************* */
#define DATABASE_PATH				("database")
#define DATABASE_FILE				("database.db")
#define DB_PATHFILE_SZ				(400)

#define SQL_COMMAND_SZ				(10000)

#define DB_USERS_TABLE				"USERS"
#define DB_MSGS_TABLE				"MSGS"
#define DB_REPORTS_TABLE			"RELATS"


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
int dbCreateAllTables(void);
int dbOpen(char *userDBPath, int flags, log_t *log);
int dbSelect(char *sqlCmd, int (*callback)(void*,int,char**,char**));
int dbInsert(char *sqlCmd);
int dbClose(void);

#endif
