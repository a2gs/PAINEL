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

int main(int argc, char *argv[])
{
	log_t log;

	if(argc != 3){
		printf("Syntax error! Usage:\n\t%s <LOG_FILE> 'WWW|XXX|YYY|ZZZ'\n\nWhere WWW, XXX, YYY and ZZZ are a combination (surrounded by \"'\" and separated by \"|\") of: REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n\n", argv[0]);
		printf("\tREDALERT = Red alert\n");
		printf("\tDBALERT = Database alert\n");
		printf("\tDBMSG = Database message\n");
		printf("\tOPALERT = Operation alert\n");
		printf("\tOPMSG = Operation message\n");
		printf("\tMSG = Just a message\n");
		printf("\tDEV = Developer (DEBUG) message\n");
		return(1);
	}

	if(logCreate(&log, argv[1], argv[2]) == LOG_NOK){
		printf("Erro criando log! [%s]\n", (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(1);
	}

	logWrite(&log, LOGDBALERT|LOGDBMSG|LOGOPALERT|LOGOPMSG, "Hello World 1!\n");
	logWrite(&log, LOGMUSTLOGIT, "Hello World 2! MUST LOG IT!\n");

	logClose(&log);

	return(0);
}
