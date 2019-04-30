/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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
#define DATABASE_PATH      ("database")
#define DATABASE_FILE      ("database.db")
#define DB_PATHFILE_SZ     (400)

#define SQL_COMMAND_SZ     (10000)

#define DB_USERS_TABLE     "USERS"
#define DB_MSGS_TABLE      "MSGS"
#define DB_REPORTS_TABLE   "RELATS"

#define SQL_NO_ROW         ('0')
#define SQL_HAS_ROW        ('1')


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */


/* *** INTERFACES / PROTOTYPES *************************************** */

void getLogSystem_DB(log_t *userLog);

/* int dbCreateAllTables(void);
 *
 * Create database and its objects: tables, index, etc.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error opening or creating database
 */
int dbCreateAllTables(void);

/* int dbOpen(char *userDBPath, int flags);
 *
 * Open database.
 *
 * INPUT:
 *  userDBPath - NULL = default system DB ($PAINEL_HOME/database/database.db)
 *  flags - 
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error opening or creating database
 */
int dbOpen(char *userDBPath, int flags/*, log_t *log*/);

/* <header function description included by another sources> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int dbSelect(char *sqlCmd, int (*callback)(void *,int ,char **,char **), void *argCallback);

/* <header function description included by another sources> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int dbInsert(char *sqlCmd);

/* int dbClose(void)
 *
 * Closes the databse.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error closing
 */
int dbClose(void);

#endif
