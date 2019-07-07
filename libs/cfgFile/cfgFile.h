#ifndef __CFG_FILE_H__
#define __CFG_FILE_H__

#include "linkedlist.h"

#define CFGFILE_OK  (0)
#define CFGFILE_NOK (1)

typedef struct _cfgFile_t{
	ll_node_t *head;
}cfgFile_t;

int cfgFileLoad(cfgFile_t *ctx, char *cfgFilePath);
int cfgFileOpt(cfgFile_t *ctx, char *label);
int cfgFileFree(cfgFile_t *ctx);

#endif
