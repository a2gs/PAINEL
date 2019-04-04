/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
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
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util.h"

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOGPATH_SZ		(350)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
char * getPAINELEnvHomeVar(void)
{
	return(getenv(PAINEL_HOME_ENV));
}

inline int changeCharByChar(char *buffer, int from, int to)
{
	char *p = NULL;

	p = strchr(buffer, from);

	if(p == NULL) return(NOK);

	*p = to;

	return(OK);
}

inline size_t cutter(char **buffer, int c, char *out, size_t outSz)
{
	register char *c1 = NULL, *c2 = NULL;
	size_t tam = 0;

	for(c1 = c2 = *buffer; (*c2 != '\0') && (*c2 != c); c2++);

	tam = c2 - c1;
	strncpy(out, c1, (outSz < tam) ? outSz : tam);

	if(*c2 == '\0') *buffer = c2;
	else            *buffer = c2+1;

	return(tam);
}

char * time_DDMMYYhhmmss(void)
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
		return(NOK);
	}

	htmls->fdStatic = fileno(htmls->htmlStatic);
	if(htmls->fdStatic == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(NOK);
	}

	/* Opening refresh */
	htmls->htmlRefresh = fopen(htmlRefresh, "w");
	if(htmls->htmlRefresh == NULL){
		fprintf(stderr, "Erro criando HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(NOK);
	}

	htmls->fdRefresh = fileno(htmls->htmlRefresh);
	if(htmls->fdRefresh == -1){
		fprintf(stderr, "Erro em pegar descritor de HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(NOK);
	}

	/* Locking */
	if(lockf(htmls->fdStatic, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML STATIC [%s]. Motivo: [%s]\n", htmlStatic, strerror(errno));
		return(NOK);
	}

	if(lockf(htmls->fdRefresh, F_LOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer lock no arquivo HTML REFRESH [%s]. Motivo: [%s]\n", htmlRefresh, strerror(errno));
		return(NOK);
	}

	return(OK);
}

int html_writeDual(htmlFiles_t *htmls, int files, char *msg)
{
	if(files == 0){
		fprintf(htmls->htmlStatic, "%s", msg);
		fprintf(htmls->htmlRefresh, "%s", msg);
	}
	else if(files == 1) fprintf(htmls->htmlStatic,  "%s", msg);
	else if(files == 2) fprintf(htmls->htmlRefresh, "%s", msg);

	return(OK);
}

int html_fflush(htmlFiles_t *htmls)
{
	fflush(htmls->htmlStatic );
	fflush(htmls->htmlRefresh);
	return(OK);
}

int html_fclose(htmlFiles_t *htmls)
{
	/* Unlocking */
	if(lockf(htmls->fdStatic, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(NOK);
	}

	if(lockf(htmls->fdRefresh, F_ULOCK, 0) < 0){
		fprintf(stderr, "Nao foi possivel fazer o unlock do arquivo HTML STATIC: [%s]\n", strerror(errno));
		return(NOK);
	}

	/* Closing */
	fclose(htmls->htmlStatic);
	fclose(htmls->htmlRefresh);

	memset(htmls, 0, sizeof(htmlFiles_t));

	return(OK);
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
static log_t *logUtil = NULL;

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

pid_t daemonizeWithoutLock(log_t *log)
{
	pid_t i = 0, father = 0;

	logUtil = NULL;

	father = getppid();
	if(father == 1)
		return(NOK);

	i = fork();

	father = i;

	if(i == -1){
		fprintf(stderr, "Erro fork chield process! [%s]\n", strerror(errno));
		return((pid_t)NOK);
	}

	if(i > 0)
		exit(0); /* Pai termina: 0 - Ok */

	/* Continuando filho... */
	setsid(); /* novo grupo */

	umask(027);

	chdir("./");

	logUtil = log;

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
