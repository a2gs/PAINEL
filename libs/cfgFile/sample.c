#include <stdio.h>

#include "cfgFile.h"

int main(int argc, char *argv[])
{
	cfgFile_t ctx;
	unsigned int lineError = 0;
	char *value = NULL;

	if(argc != 2){
		printf("Sample usage: %s <PATH_TO_CFG_FILE>\n", argv[0]);
		return(-1);
	}

	if(cfgFileLoad(&ctx, "./sample.cfg", &lineError) == CFGFILE_NOK){
	}

	if(cfgFileOpt(&ctx, "opt1", &value) == CFGFILE_NOK){
	}

	if(cfgFileFree(&ctx) == CFGFILE_NOK){
	}

	return(0);
}
