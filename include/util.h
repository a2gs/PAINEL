/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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

#include "SG.h"
#include "log.h"


/* *** DEFINES ******************************************************* */
/* PROTOCOLO (maximo MAXLINE bytes):
   PROT_COD||||||||||||| ..... |
*/

#define MAXLINE      (10000)

#define PAINEL_OK           (1)
#define PAINEL_NOK          (-1)

#define PAINEL_HOME_ENV          ("PAINEL_HOME") /* PAINEL home enviroument variable */
#define SUBPATH_RUNNING_DATA     ("running")

#define LOGOUT_CMD               ("exit")
#define LOGIN_CMD                ("login")
#define REFRESH_SEG_HTML_PAGE    "5" /* without () cuz HTML tag cannot understand */

#define LOG_PATH                 ("log")

#define STRADDR_SZ               (50)

/* Codigo de mensagens: no maximo 10 chars (variavel msgCod no serv.c), 2 ja seriam o bastante */ 
#define PROT_CODE_LEN         (10)
#define PROT_COD_PING         (0)		/* Client -> Server / Server -> Client */
#define PROT_COD_LOGIN        (1)		/* Client -> Server: login request / Server -> Client: login request OK/NOK */
#define PROT_COD_LOGOUT       (2)		/* Client -> Server: logout request / Server -> Client: logout request OK/NOK */
#define PROT_COD_IFACE        (10)		/* Client -> Server: ask user interface to build / Server -> Client: interface */
#define PROT_COD_LEVELS       (11)		/* Client -> Server: ask to server all levels */
#define PROT_COD_INSREG       (100)		/* Client -> Server: register insert / Server -> Client: register insert fail */
#define PROT_COD_SERCMD       (300)
#define PROT_COD_CLICMD       (400)		/* Client -> Server: client command / Server -> Client: client command fail */

#define DATA_LEN              (sizeof("DD/MM/YY hh:mm:ss"))

#define PINGPONG_MBL_SZ       (150)

#define HTML_FILE_LOCKED      (1)
#define HTML_FILE_UNLOCKED    (0)
#define HTML_FILE_LOCKERR     (-1)


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _netpass_t{
	char key[PASS_SHA256_ASCII_LEN + 1];
	char IV[IV_SHA256_LEN          + 1];
}netpass_t;

typedef struct _htmlFiles_t{
	FILE *htmlStatic;
	FILE *htmlRefresh;
	int fdStatic, fdRefresh;
}htmlFiles_t;

/* PROTOCOL SCHEMA */
typedef struct _protoData_t{
	char drt[DRT_LEN                         + 1];
	char passhash[PASS_SHA256_ASCII_LEN      + 1];
	char date[DATA_LEN                       + 1];
	char loginout[1                          + 1];
	char funcao[VALOR_FUNCAO_LEN             + 1];
	char panela[FORNELET_PAMELA_LEN          + 1];
	char ws[OPEMAQ_WS_LEN                    + 1];
	char fornEletr[FORNELET_NUMFORELE_LEN    + 1];
	char numMaquina[OPEMAQ_NUMMAQ_LEN        + 1];
	char diamNom[OPEMAQ_DIAMNOM_LEN          + 1];
	char classe[OPEMAQ_CLASSE_LEN            + 1];
	char temp[OPEMAQ_TEMP_LEN                + 1];
	char percFeSi[OPEMAQ_PERC_FESI_LEN       + 1];
	char percMg[FORNELET_PERC_MG_LEN         + 1];
	char percC[FORNELET_PERC_C_LEN           + 1];
	char percS[FORNELET_PERC_S_LEN           + 1];
	char percP[FORNELET_PERC_P_LEN           + 1];
	char percInoculante[OPEMAQ_PERC_INOC_LEN + 1];
	char enerEletTon[OPEMAQ_ENEELETON_LEN    + 1];
	char cadencia[SUPMAQ_CADENCIA            + 1];
	char oee[SUPMAQ_OEE                      + 1];
	char aspecto[SUPMAQ_ASPEC_LEN            + 1];
	char refugo[SUPMAG_REFUGO_LEN            + 1];
	/* char ipport[VALOR_IPPORT_LEN             + 1]; */
}protoData_t;


/* *** INTERFACES / PROTOTYPES *************************************** */

/* int formatProtocol(protoData_t *data, int protoCmd, char *msg, size_t msgSzIn, size_t *msgSzOut);
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
int formatProtocol(protoData_t *data, int protoCmd, char *msg, size_t msgSzIn, size_t *msgSzOut);

/* void getLogSystem_Util(log_t *logClient)
 *
 * Save the instance to log system for util functions.
 *
 * INPUT:
 *  logClient - Log opened by client engine.
 * OUTPUT:
 *  none
 */
void getLogSystem_Util(log_t *log);

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

/* int daemonizeWithoutLock(void)
 *
 * Daemonizes the process without create a 'process lock file' (guaranteeing only one instance).
 *
 * INPUT:
 * OUTPUT:
 *  pid_t - Daemon PID number, 1 to father or PAINEL_NOK if fork() error.
 */
int daemonizeWithoutLock(void);

/* inline int changeCharByChar(char *buffer, int to, int from)
 *
 *
 * INPUT:
 *  buffer - String to search and replace char
 *  from - char to be replaced
 *  to - char to replace
 * OUTPUT:
 *  PAINEL_OK - ONE char had been replaced.
 *  PAINEL_NOK - None char found.
 */
int changeCharByChar(char *buffer, int to, int from);

/* inline size_t cutter(char **buffer, int c, char *out, size_t outSz)
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
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error (erro or open a file or locking the files)
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
 *  PAINEL_OK - OK
 *  PAINEL_NOK - Write error
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
 *  PAINEL_OK - Ok
 */
int html_fflush(htmlFiles_t *htmls);

/* int html_fclose(htmlFiles_t *htmls)
 *
 * Closes the HTML files.
 *
 * INPUT:
 *  htmls - HTML files
 * OUTPUT:
 *  PAINEL_OK - Ok
 *  PAINEL_NOK - Error unlocking the files
 */
int html_fclose(htmlFiles_t *htmls);

/* int html_fclose(htmlFiles_t *htmls)
 *
 * Removes spaces and tabs before and after a string.
 *
 * INPUT:
 *  strIn - String to remove
 *  szSrtOut - Size of strOut
 *  
 * OUTPUT:
 *  strOut - 
 *  Size of strOut copied (strlen(strOut)).
 */
size_t alltrim(char *strIn, char *strOut, size_t szSrtOut);

/* size_t n_strncpy(char *dest, const char *src, size_t n)
 *
 * Like strncpy, but return the number of bytes copied until *src first '\0' or n bytes.
 *
 * INPUT:
 *  src - Source string
 *  n - Max size of *dest
 *  
 * OUTPUT:
 *  dest - String copied
 *  size_t - Number of bytes copied
 */
size_t n_strncpy(char *dest, const char *src, size_t n); /* man strncpy(3) */

/* int encrypt_SHA256(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertextSz)
 *
 *
 *
 * INPUT:
 *  
 * OUTPUT:
 * PAINEL_OK - 
 * PAINEL_NOK - 
 * ciphertext - 
 * ciphertextSz - 
 */
int encrypt_SHA256(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertextSz);

/* int decrypt_SHA256(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *ciphertextSz)
 *
 *
 * INPUT:
 *  
 * OUTPUT:
 * PAINEL_OK - 
 * PAINEL_NOK - 
 * plaintext - 
 * plaintextSz - 
 */
int decrypt_SHA256(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, int *plaintextSz);

int calcHashedNetKey(char *plan, char*hashed);

#endif
