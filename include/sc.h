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


/* *** DEFINES ******************************************************* */
/* PROTOCOLO (maximo MAXLINE bytes):
   PROT_COD||||||||||||| ..... |
*/

#define MAXLINE		(10000)

#define OK				(1)
#define NOK				(-1)

#define LOGOUT_CMD					("exit")
#define LOGIN_CMD						("login")
#define REFRESH_SEG_HTML_PAGE		"5" /* sem () */

#define LOG_PATH			"./log/" /* sem () */

/* Codigo de mensagens: no maximo 10 chars (variavel msgCod no serv.c), 2 ja seriam o bastante */ 
#define PROT_CODE_LEN			(10)
#define PROT_COD_LOGIN			(1)
#define PROT_COD_LOGOUT			(2)
#define PROT_COD_INSREG			(100)
#define PROT_COD_SERCMD			(300)
#define PROT_COD_CLICMD			(400)

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
/* int log_open(char *logFile, FILE *log)
 *
 * Initializes the logging system.
 *
 * INPUT:
 *  logFile - path to log file
 * OUTPUT:
 *  log - log file handler
 *  OK - Ok
 *  NOK - Error (unable to create file)
 */
int log_open(char *logFile, FILE *log);

/* int log_close(FILE *log)
 *
 * Closes the logging system.
 *
 * INPUT:
 *  log - log file handler
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error (unable to close file)
 */
int log_close(FILE *log);

/* int log_write(char *logFile, char *msg, ...)
 *
 * Writes a message to log, with printf() style usage.
 * TODO: use log_open(FILE *log) parameter (at the moment, this function open/write/FLUSH THE BUFFER and close the file: ensuring consistency)
 *
 * INPUT:
 *  logFile - Path to log file. 
 *  msg - Message template
 *  ... - variables
 * OUTPUT:
 *  OK - Ok
 *  NOK - Unable to open or write to log
 */
int log_write(char *logFile, char *msg, ...);

/* int daemonizeWithoutLock(void)
 *
 * Daemonizes the process without create a 'process lock file' (guaranteeing only one instance).
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  pid_t - Daemon PID number, 1 to father or NOK if fork() error.
 */
int daemonizeWithoutLock(void);

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
