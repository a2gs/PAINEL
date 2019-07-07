#include <string.h>

#include "cfgFile.h"
#include "linkedlist.h"

typedef struct _cfgFileNode_t{
	char *label;
	char *value;
}cfgFileNode_t;

int cfgFileLoad(cfgFile_t *ctx, char *cfgFilePath)
{
	return(CFGFILE_OK);
}

int cfgFileOpt(cfgFile_t *ctx, char *label)
{
	return(CFGFILE_OK);
}

int cfgFileFree(cfgFile_t *ctx)
{
	return(CFGFILE_OK);
}
