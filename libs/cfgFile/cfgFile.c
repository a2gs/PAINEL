#include <string.h>

#include "cfgFile.h"
#include "linkedlist.h"

/*
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
*/

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
