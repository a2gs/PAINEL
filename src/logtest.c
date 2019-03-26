#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "log.h"

#define OK 0
#define NOK 1

/* --- HEADER START --------------------------------------------------------------- */

/*
exec REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV
*/

typedef struct _log_t{
	unsigned int level;
	int fd;
}log_t;

typedef struct _logLevel_t{
	int value;
	char *name;
}logLevel_t;

logLevel_t levels[] = {
	{.value = LOG_RED_ALERT,        .name = "|REDALERT|"},
	{.value = LOG_DATABASE_ALERT,   .name = "|DBALERT|" },
	{.value = LOG_DATABASE_MESSAGE, .name = "|DBMSG|"   },
	{.value = LOG_OPERATOR_ALERT,   .name = "|OPALERT|" },
	{.value = LOG_OPERATOR_MESSAGE, .name = "|OPMSG|"   },
	{.value = LOG_MESSAGE,          .name = "|MSG|"     },
	{.value = LOG_DEVELOP,          .name = "|DEV|"     }
};

int parsingLogCmdLine(char *cmdLog, unsigned int *level)
{
#define LOG_CMDMASK_SZ           (sizeof("|REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV|"))
#define LOG_TOTAL_LEVELS_DEFINED (sizeof(levels)/sizeof(logLevel_t))
	unsigned int i = 0;
	char mask[LOG_CMDMASK_SZ + 1] = {'\0'};

	snprintf(mask, LOG_CMDMASK_SZ, "|%s|", cmdLog);

	for(*level = LOG_MUST_LOG_IT, i = 0; i < LOG_TOTAL_LEVELS_DEFINED; i++){
		if(strstr(mask, levels[i].name) != NULL)
			*level |= levels[i].value;
	}

	return(OK);
}

int logWrite(log_t *log, unsigned int msgLevel, char *msg)
{
	if(log->level & msgLevel)
		printf("DEBUG: [%02x]: %s", msgLevel, msg);


	return(OK);
}

int logCreate(log_t *log, char *fullPath, char *cmdLog)
{
	log->level = 0;
	log->fd = 0;
	errno = 0;

	if(parsingLogCmdLine(cmdLog, &(log->level)) == NOK)
		return(NOK);

	printf("DEBUG: level defined: [%02X]\n", log->level);

	if((log->fd = open(fullPath, O_WRONLY|O_CREAT|O_APPEND|O_NONBLOCK, S_IRUSR|S_IWUSR|S_IRGRP)) == -1)
		return(NOK);

	return(OK);
}

int logClose(log_t *log)
{
	close(log->fd);
	return(OK);
}

/* --- HEADER END --------------------------------------------------------------- */

int main(int argc, char *argv[])
{
	log_t log;

	printf("REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n\n");

	if(argc != 2){
		printf("Erro de argumento\n");
		return(1);
	}

	if(logCreate(&log, "./log.text", argv[1]) == NOK){
		printf("Erro criando log! [%s]\n", (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(1);
	}

	logWrite(&log, LOG_DATABASE_ALERT|LOG_DATABASE_MESSAGE|LOG_OPERATOR_ALERT|LOG_OPERATOR_MESSAGE, "ola mundo\n");
	logWrite(&log, LOG_MUST_LOG_IT, "ola mundo OBRIGATIORIO\n");


	logClose(&log);

	return(0);
}
