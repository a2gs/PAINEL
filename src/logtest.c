#include <stdio.h>
#include <string.h>
#include "log.h"

#define OK 0
#define NOK 1

#define LEVEL_MUST_LOG_IT        (unsigned int)0x1
#define LEVEL_RED_ALERT          (unsigned int)0x2
#define LEVEL_DATABASE_ALERT     (unsigned int)0x4
#define LEVEL_DATABASE_MESSAGE   (unsigned int)0x8
#define LEVEL_OPERATOR_ALERT     (unsigned int)0x10
#define LEVEL_OPERATOR_MESSAGE   (unsigned int)0x20
#define LEVEL_MESSAGE            (unsigned int)0x40
#define LEVEL_DEVELOP            (unsigned int)0x80

/*
exec REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV
*/

typedef struct _logLevel_t{
	int value;
	char *name;
}logLevel_t;

logLevel_t levels[] = {
	{.value = LEVEL_RED_ALERT,        .name = "REDALERT"},
	{.value = LEVEL_DATABASE_ALERT,   .name = "DBALERT"},
	{.value = LEVEL_DATABASE_MESSAGE, .name = "DBMSG"},
	{.value = LEVEL_OPERATOR_ALERT,   .name = "OPALERT"},
	{.value = LEVEL_OPERATOR_MESSAGE, .name = "OPMSG"},
	{.value = LEVEL_MESSAGE,          .name = "MSG"},
	{.value = LEVEL_DEVELOP,          .name = "DEV"}
};

#define LOG_TOTAL_LEVELS_DEFINED		(sizeof(levels)/sizeof(logLevel_t))

int parsingLogCmdLine(char *cmdLog, unsigned int *level)
{
	unsigned int i = 0;

	for(*level = LEVEL_MUST_LOG_IT, i = 0; i < LOG_TOTAL_LEVELS_DEFINED; i++){
		if(strstr((const char *)cmdLog, levels[i].name) != NULL)
			*level |= levels[i].value;
	}

	return(OK);
}

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
