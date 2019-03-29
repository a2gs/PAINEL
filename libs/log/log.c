/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* log.c
 * Log system.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define LOG_FMTMSG_SZ            (5000)
#define LOG_TOTAL_LEVELS_DEFINED (sizeof(levels)/sizeof(logLevel_t))

typedef struct _logLevel_t{
	int value;
	char *name;
}logLevel_t;

logLevel_t levels[] = {
	{.value = LOGREDALERT, .name = "|REDALERT|"},
	{.value = LOGDBALERT,  .name = "|DBALERT|" },
	{.value = LOGDBMSG,    .name = "|DBMSG|"   },
	{.value = LOGOPALERT,  .name = "|OPALERT|" },
	{.value = LOGOPMSG,    .name = "|OPMSG|"   },
	{.value = LOGMSG,      .name = "|MSG|"     },
	{.value = LOGDEV,      .name = "|DEV|"     }
};


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int parsingLogCmdLine(char *cmdLog, unsigned int *level)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int parsingLogCmdLine(char *cmdLog, unsigned int *level)
{
	unsigned int i = 0;
	char mask[LOG_CMDMASK_SZ + 1] = {'\0'};

	snprintf(mask, LOG_CMDMASK_SZ, "|%s|", cmdLog);

	for(*level = LOGMUSTLOGIT, i = 0; i < LOG_TOTAL_LEVELS_DEFINED; i++){
		if(strstr(mask, levels[i].name) != NULL)
			*level |= levels[i].value;
	}

	return(LOG_OK);
}

int logWrite(log_t *log, unsigned int msgLevel, char *msg, ...)
{
	int totFmt = sizeof("YYYYMMDD HHMMSS");
	va_list args;
	struct tm logTimeTm;
	time_t logTimeTimet;
	struct timeval tv;
	char fmtMsg[LOG_FMTMSG_SZ + 1] =  {'\0'};

	errno = 0;

	if(log == NULL)   return(LOG_NOK);
	if(log->fd == -1) return(LOG_NOK);

	time(&logTimeTimet);
	memcpy(&logTimeTm, localtime(&logTimeTimet), sizeof(struct tm));
	strftime(fmtMsg, LOG_FMTMSG_SZ, "%Y%m%d %H%M%S", &logTimeTm);

	if(gettimeofday(&tv, NULL) == -1)
		return(LOG_NOK);

	va_start(args, msg);

	totFmt += snprintf(&(fmtMsg[totFmt - 1]), LOG_FMTMSG_SZ - totFmt, " %06ld|%06d|", tv.tv_usec, getpid());

	totFmt += vsnprintf(&(fmtMsg[totFmt - 1]), LOG_FMTMSG_SZ - totFmt, msg, args);

	if(log->level & msgLevel){
		if(write(log->fd, fmtMsg, totFmt - 1) == -1){
			va_end(args);
			return(LOG_NOK);
		}
	}

	va_end(args);
	return(LOG_OK);
}

int logCreate(log_t *log, char *fullPath, char *cmdLog)
{
	log->level = 0;
	log->fd = -1;
	errno = 0;

	if(log == NULL) return(LOG_NOK);

	if(parsingLogCmdLine(cmdLog, &(log->level)) == LOG_NOK)
		return(LOG_NOK);

	if((log->fd = open(fullPath, O_WRONLY|O_CREAT|O_APPEND|O_NONBLOCK, S_IRUSR|S_IWUSR|S_IRGRP)) == -1)
		return(LOG_NOK);

	return(LOG_OK);
}

int logClose(log_t *log)
{
	if(log == NULL)   return(LOG_NOK);
	if(log->fd == -1) return(LOG_NOK);

	close(log->fd);
	return(LOG_OK);
}
