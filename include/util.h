/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* sc.h
 * Project's auxiliary functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __SC_H__
#define __SC_H__


/* *** INCLUDES ****************************************************** */
#include <stdio.h>

#include "log.h"


/* *** DEFINES ******************************************************* */
/* PROTOCOLO (maximo MAXLINE bytes):
   PROT_COD||||||||||||| ..... |
*/

#define MAXLINE		(10000)

#define OK				(1)
#define NOK				(-1)

#define PAINEL_HOME_ENV				("PAINEL_HOME") /* PAINEL home enviroument variable */
#define SUBPATH_RUNNING_DATA		("running")

#define LOGOUT_CMD					("exit")
#define LOGIN_CMD						("login")
#define REFRESH_SEG_HTML_PAGE		"5" /* without () cuz HTML tag cannot understand */

#define LOG_PATH						("log")

/* Codigo de mensagens: no maximo 10 chars (variavel msgCod no serv.c), 2 ja seriam o bastante */ 
#define PROT_CODE_LEN			(10)
#define PROT_COD_LOGIN			(1)		/* Client -> Server: login request / Server -> Client: login request OK/NOK */
#define PROT_COD_LOGOUT			(2)		/* Client -> Server: logout request / Server -> Client: logout request OK/NOK */
#define PROT_COD_INSREG			(100)		/* Client -> Server: register insert / Server -> Client: register insert fail */
/* #define PROT_COD_SERCMD			(300) */
#define PROT_COD_CLICMD			(400)		/* Client -> Server: client command / Server -> Client: client command fail */

#define DATA_LEN						(sizeof("DD/MM/YY hh:mm:ss"))

#define HTML_FILE_LOCKED		(1)
#define HTML_FILE_UNLOCKED		(0)
#define HTML_FILE_LOCKERR		(-1)


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _htmlFiles_t{
	FILE *htmlStatic;
	FILE *htmlRefresh;
	int fdStatic, fdRefresh;
}htmlFiles_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* char * getPAINELEnvHomeVar(void)
 *
 * Return a poiter to PAINEL_HOME enviroument variable.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  Like getenv(3): pointer to env varible or NULL if not defined.
 */
char * getPAINELEnvHomeVar(void);

/* int log_open(char *logFile)
 *
 * Initializes the logging system.
 *
 * INPUT:
 *  logFile - path to log file
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error (unable to create file)
int log_open(char *logFile);
 */

/* int log_close(void)
 *
 * Closes the logging system.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error (unable to close file)
int log_close(void);
 */

/* int log_write(char *msg, ...)
 *
 * Writes a message to log, with printf() style usage.
 *
 * INPUT:
 *  msg - Message template
 *  ... - variables
 * OUTPUT:
 *  OK - Ok
 *  NOK - Unable to open or write to log
int log_write(char *msg, ...);
 */

/* int daemonizeWithoutLock(void)
 *
 * Daemonizes the process without create a 'process lock file' (guaranteeing only one instance).
 *
 * INPUT:
 *  log - Pointer to log system
 * OUTPUT:
 *  pid_t - Daemon PID number, 1 to father or NOK if fork() error.
 */
int daemonizeWithoutLock(log_t *log);

/* size_t cutter(char **buffer, int c, char *out, size_t outSz)
 *
 * Parsing a buffer with c delimiter, copy to out and forward buffer pointer.
 * If delimiter was researched, buffer will point to the next char (after delimiter).
 * If not researched, buffer will point to yourself '\0' (buffer MUST BE A '\0' TERMINATED STRING).
 * (This function works like strchrnul(), from GNU lib)
 *
 * INPUT:
 *  buffer - String to parse
 *  c - delimiter
 *  outSz - Max size of out
 * OUTPUT:
 *  buffer - Byte' address after delimiter ('\0' from the same string if not researched)
 *  out - Token pointed from buffer until delimiter
 *  return - The token' size
 */
size_t cutter(char **buffer, int c, char *out, size_t outSz);

/* char * time_DDMMYYhhmmss(void)
 *
 * Return a static string with data/time format DDMMYYhhmmss.
 * It is a MT-Unsafe (see attributes(7)) function.
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  Address to a formatted data/time buffer (with '\0' at end).
 */
char * time_DDMMYYhhmmss(void);

/* int html_fopen(htmlFiles_t *htmls, char *htmlStatic, char *htmlRefresh)
 *
 * Open and lock the static and refresh HTML files into htmlFiles_t.
 *
 * INPUT:
 *  htmlStatic - Path to HTML static file
 *  htmlRefresh - Path to HTML refresh file
 * OUTPUT:
 *  htmls - HTMLs handles
 *  OK - Ok
 *  NOK - Error (erro or open a file or locking the files)
 */
int html_fopen(htmlFiles_t *htmls, char *htmlStatic, char *htmlRefresh);

/* int html_writeDual(htmlFiles_t *htmls, int files, char *msg)
 *
 * Write a message to HTMLs files.
 *
 * INPUT:
 *  htmls - HTMLs files
 *  files - 0 = Write into both files | 1 = Write into static file | 2 = Write to refresh file
 *  msg - String to be written
 * OUTPUT:
 *  OK - OK
 *  NOK - Write error
 */
int html_writeDual(htmlFiles_t *htmls, int files, char *msg);

/* int html_testHtmlLock(FILE *file)
 *
 * Test if a file is locked.
 *
 * INPUT:
 *  file - File handle
 * OUTPUT:
 *  HTML_FILE_UNLOCKED - File not locked
 *  HTML_FILE_LOCKED - File locked
 *  HTML_FILE_LOCKERR - Error trying test the lock
 */
int html_testHtmlLock(FILE *file);

/* int html_fflush(htmlFiles_t *htmls)
 *
 * Flushes the HTML files buffers.
 *
 * INPUT:
 *  htmls - HTML files
 * OUTPUT:
 *  OK - Ok
 */
int html_fflush(htmlFiles_t *htmls);

/* int html_fclose(htmlFiles_t *htmls)
 *
 * Closes the HTML files.
 *
 * INPUT:
 *  htmls - HTML files
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error unlocking the files
 */
int html_fclose(htmlFiles_t *htmls);

#endif
