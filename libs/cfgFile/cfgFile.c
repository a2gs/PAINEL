#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cfgFile.h"
#include "linkedlist.h"

typedef struct _cfgFileNode_t{
	char *label;
	char *value;
}cfgFileNode_t;

size_t alltrim(char *strIn, char *strOut, size_t szSrtOut)
{
   char *init = NULL, *end = NULL;
   size_t toCopy = 0;

   for(init = strIn; (*init == ' ' || *init == '\t'); init++);

   end = strrchr(init, '\0');
   if(end == NULL)
      return(0);

   for(end--; (*end == ' ' || *end == '\t'); end--);

   if((size_t)(end - init) < szSrtOut) toCopy = end - init + 1;
   else toCopy = szSrtOut;

   memcpy(strOut, init, toCopy);
   strOut[toCopy] = '\0';

   return(toCopy);
}

int cfgFileLoad(cfgFile_t *ctx, char *cfgFilePath, unsigned int *lineError)
{
   FILE *f = NULL;
   char *c = NULL;
	cfgFileNode_t *newNode = NULL;
   char line    [CFGFILE_LINE_SZ  + 1] = {'\0'};
   char labelAux[CFGFILE_LABEL_SZ + 1] = {'\0'};
   char valueAux[CFGFILE_VALUE_SZ + 1] = {'\0'};
	size_t labelAuxSz = 0;
	size_t valueAuxSz = 0;

	lineError = 0;

   f = fopen(cfgFilePath, "r");
   if(f == NULL){
      return(CFGFILE_NOK);
   }

	ctx->head = NULL;
	ll_create(&ctx->head);

   for(*lineError = 1; fgets(line, CFGFILE_LINE_SZ, f) != NULL; *lineError++){
      if(line[0] == '#' || line[0] == '\0' || line[0] == '\n' || line[0] == '\r' || line[0] == '=') continue;

      c = strchr(line, '\n');
      if(c != NULL) *c = '\0';

      c = strchr(line, '=');
      if(c == NULL) continue;

      labelAuxSz = alltrim(line , labelAux, c - line);
      valueAuxSz = alltrim(c + 1, valueAux, CFGFILE_VALUE_SZ);

		newNode = (cfgFileNode_t *)malloc(sizeof(cfgFileNode_t));
		if(newNode == NULL){
      	return(CFGFILE_NOK); /* TODO: free previous allocated memory */
		}

		newNode->label = (char *)malloc(labelAuxSz);
		if(newNode->label == NULL){
      	return(CFGFILE_NOK); /* TODO: free previous allocated memory */
		}

		newNode->value = (char *)malloc(valueAuxSz);
		if(newNode->value == NULL){
      	return(CFGFILE_NOK); /* TODO: free previous allocated memory */
		}


		/*
      if(strcmp(valueAux, opt) == 0){

         *cfgSzRead = alltrim(c + 1, cfg, cfgSz);

         fclose(f);
         return(PAINEL_OK);
      }
		*/
   }



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

int cfgFileReadValue(char *pathCfg, char *opt, char *cfg, size_t cfgSz, size_t *cfgSzRead)
{
	FILE *f = NULL;
	char *c = NULL;
	char line   [CFGFILE_LINE_SZ  + 1] = {'\0'};
	char optLine[CFGFILE_VALUE_SZ + 1] = {'\0'};

	f = fopen(pathCfg, "r");
	if(f == NULL){
		return(CFGFILE_NOK);
	}

	*cfgSzRead = 0;

	for( ; fgets(line, CFGFILE_LINE_SZ, f) != NULL; ){
		if(line[0] == '#' || line[0] == '\0' || line[0] == '\n' || line[0] == '\r' || line[0] == '=') continue;

		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';

		c = strchr(line, '=');
		if(c == NULL) continue;

		alltrim(line, optLine, c - line);

		if(strcmp(optLine, opt) == 0){

			*cfgSzRead = alltrim(c + 1, cfg, cfgSz);

			fclose(f);
			return(CFGFILE_OK);
		}
	}

	fclose(f);
	return(CFGFILE_OK);
}
