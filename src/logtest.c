#include <stdio.h>
#include <string.h>
#include "log.h"

#define OK 0
#define NOK 1

/* --- HEADER START --------------------------------------------------------------- */

/*
exec REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV
*/

typedef struct _logLevel_t{
	int value;
	char *name;
}logLevel_t;

logLevel_t levels[] = {
	{.value = LOG_RED_ALERT,        .name = "REDALERT"},
	{.value = LOG_DATABASE_ALERT,   .name = "DBALERT"},
	{.value = LOG_DATABASE_MESSAGE, .name = "DBMSG"},
	{.value = LOG_OPERATOR_ALERT,   .name = "OPALERT"},
	{.value = LOG_OPERATOR_MESSAGE, .name = "OPMSG"},
	{.value = LOG_MESSAGE,          .name = "MSG"},
	{.value = LOG_DEVELOP,          .name = "DEV"}
};

#define LOG_TOTAL_LEVELS_DEFINED		(sizeof(levels)/sizeof(logLevel_t))

/* --- HEADER END --------------------------------------------------------------- */

int parsingLogCmdLine(char *cmdLog, unsigned int *level)
{
	unsigned int i = 0;

	for(*level = LOG_MUST_LOG_IT, i = 0; i < LOG_TOTAL_LEVELS_DEFINED; i++){
		if(strstr((const char *)cmdLog, levels[i].name) != NULL)
			*level |= levels[i].value;
	}

	return(OK);
}

/* --- */

int main(int argc, char *argv[])
{
	unsigned int logLevel = 0;

	printf("REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n\n");

	if(argc != 2){
		printf("Erro de argumento\n");
		return(1);
	}

	if(parsingLogCmdLine(argv[1], &logLevel) == NOK){
		printf("Erro parsingLogCmdLine()\n");
		return(1);
	}

	printf("level: [%02x]\n", logLevel);





	return(0);
}
