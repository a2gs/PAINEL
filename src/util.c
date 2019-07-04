/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* util.c
 * Project's auxiliary functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
/*
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
*/

#include "util.h"

#include <log.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOGPATH_SZ		(350)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */
static log_t *logUtil = NULL;


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
char * getPAINELEnvHomeVar(void)
{
	return(getenv(PAINEL_HOME_ENV));
}

void getLogSystem_Util(log_t *log)
{
	logUtil = log;
	return;
}

inline int changeCharByChar(char *buffer, int from, int to)
{
	char *p = NULL;

	p = strchr(buffer, from);

	if(p == NULL) return(PAINEL_NOK);

	*p = to;

	return(PAINEL_OK);
}

inline size_t cutter(char **buffer, int c, char *out, size_t outSz)
{
	char *c1 = NULL, *c2 = NULL;
	size_t tam = 0, less = 0;

	for(c1 = c2 = *buffer; (*c2 != '\0') && (*c2 != c); c2++);

	tam = c2 - c1;
	less = ((outSz < tam) ? outSz : tam);
	strncpy(out, c1, less);

	out[less] = '\0';

	if(*c2 == '\0') *buffer = c2;
	else            *buffer = c2+1;

	return(tam);
}

inline char * time_DDMMYYhhmmss(void)
{
	static char segmil[DATA_LEN + 1] = {'\0'};
	time_t rawtime;
	struct tm *tmTime = NULL;

	rawtime = time(0);
	tmTime = localtime(&rawtime);
	strftime((char *)segmil, DATA_LEN, "%d/%m/%y %H:%M:%S", tmTime);

	segmil[DATA_LEN - 1]='\0';

	return(segmil);
}

/* ---[DUAL HTML]------------------------------------------------------------------------------------------ */
int html_fopen(htmlFiles_t *htmls, char *htmlStatic, char *htmlRefresh)
{
	/* Opening static */
	htmls->htmlStatic = fopen(htmlStatic, "w");
	if(htmls->htmlStatic == NULL){
		fprintf(stderr, "Erro criando HTML STATIC [%s]. Motivo [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	htmls->fdStatic = fileno(htmls->htmlStatic);
	if(htmls->fdStatic == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	/* Opening refresh */
	htmls->htmlRefresh = fopen(htmlRefresh, "w");
	if(htmls->htmlRefresh == NULL){
		fprintf(stderr, "Erro criando HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	htmls->fdRefresh = fileno(htmls->htmlRefresh);
	if(htmls->fdRefresh == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	/* Locking */
	if(lockf(htmls->fdStatic, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(PAINEL_NOK);
	}

	if(lockf(htmls->fdRefresh, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(PAINEL_NOK);
	}

	return(PAINEL_OK);
}

int html_writeDual(htmlFiles_t *htmls, int files, char *msg)
{
	if(files == 0){
		fprintf(htmls->htmlStatic, "%s", msg);
		fprintf(htmls->htmlRefresh, "%s", msg);
	}
	else if(files == 1) fprintf(htmls->htmlStatic,  "%s", msg);
	else if(files == 2) fprintf(htmls->htmlRefresh, "%s", msg);

	return(PAINEL_OK);
}

int html_fflush(htmlFiles_t *htmls)
{
	fflush(htmls->htmlStatic );
	fflush(htmls->htmlRefresh);
	return(PAINEL_OK);
}

int html_fclose(htmlFiles_t *htmls)
{
	/* Unlocking */
	if(lockf(htmls->fdStatic, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(PAINEL_NOK);
	}

	if(lockf(htmls->fdRefresh, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(PAINEL_NOK);
	}

	/* Closing */
	fclose(htmls->htmlStatic);
	fclose(htmls->htmlRefresh);

	memset(htmls, 0, sizeof(htmlFiles_t));

	return(PAINEL_OK);
}

int html_testHtmlLock(FILE *file)
{
	int fd = 0;

	fd = fileno(file);
	if(fd == -1){
		fprintf(stderr, "Erro fileno: %s\n", strerror(errno));
		return(HTML_FILE_LOCKERR);
	}

	if(lockf(fd, F_TEST, 0) < 0)
		return(HTML_FILE_UNLOCKED);

	return(HTML_FILE_UNLOCKED);
}

/* ---[DAEMONIZE]------------------------------------------------------------------------------------------ */
/* void signal_handlerWithoutLock(int sig)
 *
 * Daemon signal handler (this function has no 'process lock file' to unlock).
 *
 * INPUT:
 *  sig - signal received
 * OUTPUT:
 *  none
 */
void signal_handlerWithoutLock(int sig)
{
	logWrite(logUtil, LOGMUSTLOGIT, "Got signal [%d] at [%s]!\n", sig, time_DDMMYYhhmmss());

	switch(sig){
		case SIGHUP:
			break;
		case SIGTERM:
			break;
	}

	if(logUtil != NULL)
		logClose(logUtil);

	if(logUtil->fd != -1)
		logClose(logUtil);

	exit(0);
	return;
}

pid_t daemonizeWithoutLock(void)
{
	pid_t i = 0, father = 0;

	/* logUtil = NULL; */

	father = getppid();
	if(father == 1)
		return(PAINEL_NOK);

	i = fork();

	father = i;

	if(i == -1){
		fprintf(stderr, "Erro fork chield process! [%s]\n", strerror(errno));
		return((pid_t)PAINEL_NOK);
	}

	if(i > 0)
		exit(0); /* Pai termina: 0 - Ok */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	chdir("./");

	/* logUtil = log; */

	/* Configurando sinais */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP, signal_handlerWithoutLock);
	signal(SIGTERM, signal_handlerWithoutLock);
	signal(SIGPIPE, signal_handlerWithoutLock);

	return(getpid());
}

size_t alltrim(char *strIn, char *strOut, size_t szSrtOut)
{
	char *init = NULL, *end = NULL;
	size_t toCopy = 0;

	for(init = strIn; (*init == ' ' || *init == '\t'); init++);

	end = strrchr(init, '\0');
	if(end == NULL)
		return(0);

	for(end--; (*end == ' ' || *end == '\t'); end--);

	if((size_t)(end - init) < szSrtOut) toCopy = end - init + 1;
	else toCopy = szSrtOut;

	memcpy(strOut, init, toCopy);
	strOut[toCopy] = '\0';

	return(toCopy);
}

size_t n_strncpy(char *dest, const char *src, size_t n) /* man strncpy(3) */
{
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];

	dest[i] = '\0';

	return(i);
}

int formatProtocol(protoData_t *data, int protoCmd, char *msg, size_t msgSzIn, size_t *msgSzOut)
{
	memset(msg, 0, msgSzIn);

	switch(protoCmd){
		case PROT_COD_PING:
			break;

		case PROT_COD_LOGIN:
			*msgSzOut = snprintf(msg, msgSzIn, "%d|%s|%s|%s|%s", protoCmd, data->drt, time_DDMMYYhhmmss(), data->funcao, data->passhash);
			break;

		case PROT_COD_LOGOUT:
			break;

		case PROT_COD_IFACE:
			*msgSzOut = snprintf(msg, msgSzIn, "%d|", protoCmd);
			break;

		case PROT_COD_LEVELS:
			*msgSzOut = snprintf(msg, msgSzIn, "%d|", protoCmd);
			break;

		case PROT_COD_INSREG:
			*msgSzOut = snprintf(msg, msgSzIn,
			                     "%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			                     protoCmd,         data->drt,            data->date,        data->loginout,
										data->funcao,     data->panela,         data->ws,          data->fornEletr,
			                     data->numMaquina, data->diamNom,        data->classe,      data->temp,
			                     data->percFeSi,   data->percMg,         data->percC,       data->percS,
			                     data->percP,      data->percInoculante, data->enerEletTon, data->cadencia,
			                     data->oee,        data->aspecto,        data->refugo);
			break;

		case PROT_COD_SERCMD:
			break;

		case PROT_COD_CLICMD:
			break;

		default:
			return(PAINEL_NOK);
			break;
	}

	return(PAINEL_OK);
}

int cfgReadOpt(char *pathCfg, char *opt, char *cfg, size_t cfgSz, size_t *cfgSzRead)
{
	FILE *f = NULL;
	char *c = NULL;
	unsigned int i = 0;
	char line[LINE_CFG_BUFF_SZ + 1] = {'\0'};
	char optLine[OPTLINE_CFG_BUFF_SZ + 1] = {'\0'};

	f = fopen(pathCfg, "r");
	if(f == NULL){
		logWrite(logUtil, LOGOPALERT, "Unable to open file (only-read) [%s] to read [%s] value: [%s].\n", pathCfg, opt, strerror(errno));
		return(PAINEL_NOK);
	}

	*cfgSzRead = 0;

	for(i = 0; fgets(line, LINE_CFG_BUFF_SZ, f) != NULL; i++){
		if(line[0] == '#' || line[0] == '\0' || line[0] == '\n' || line[0] == '\r' || line[0] == '=') continue;

		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		c = strchr(line, '=');
		if(c == NULL) continue;

		alltrim(line, optLine, c - line);

		if(strcmp(optLine, opt) == 0){

			*cfgSzRead = alltrim(c + 1, cfg, cfgSz);

			fclose(f);
			return(PAINEL_OK);
		}
	}

	fclose(f);
	return(PAINEL_OK);
}
