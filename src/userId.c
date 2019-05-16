/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* userId.c
 * Functions to manage userId (DRT) to local file DRT_FILE.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 03/05/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "userId.h"

#include <linkedlist.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
int dumpUserIdMemoryFromFile(ll_node_t **head, char *userIdFullPath)
{
	char *c = NULL;
	FILE *fUserIdDRT = NULL;
	userId_t *newNode = NULL;
	char drt[DRT_LEN + 1] = {'\0'};
	char line[LINE_DRT_FILE_LEN + 1] = {'\0'};
	char funcao[VALOR_FUNCAO_LEN + 1] = {'\0'};

	if((fUserIdDRT = fopen(userIdFullPath, "w")) == NULL){
		return(PAINEL_NOK);
	}

	while(!feof(fUserIdDRT)){
		memset(line, '\0', LINE_DRT_FILE_LEN + 1);

		if(fgets(line, LINE_DRT_FILE_LEN, fUserIdDRT) == NULL)
			break;

		if(line[0] == '#') /* Comment */
			continue;

		c = strchr(line, '-');
		if(c == NULL)
			continue; /* linha mal formatada, sem '-' */

		changeCharByChar(line, '\n', '\0');

		newNode = (userId_t *)malloc(sizeof(userId_t));
		if(newNode == NULL){
			fclose(fUserIdDRT);
			return(PAINEL_NOK);
		}

		memset(newNode,  0 , sizeof(userId_t)    );
		memset(drt,    '\0', DRT_LEN          + 1);
		memset(funcao, '\0', VALOR_FUNCAO_LEN + 1);

		c = line;
		cutter(&c, '-', drt, DRT_LEN);
		if(*c == '\0') return(PAINEL_NOK);

		cutter(&c, '\0', funcao, VALOR_FUNCAO_LEN);

		strncpy(newNode->userId, drt, DRT_LEN);
		newNode->level = string_2_UserType_t(funcao);

		if(ll_add(&head, newNode) == LL_NOK){
			fclose(fUserIdDRT);
			ll_destroyList(&head);
			return(PAINEL_NOK);
		}
	}

	fclose(fUserIdDRT);
	return(PAINEL_OK);
}

int loadUserIdFileToMemory(ll_node_t **head, char *userIdFullPath)
{
	char *c = NULL;
	FILE *fUserIdDRT = NULL;
	userId_t *newNode = NULL;
	char drt[DRT_LEN + 1] = {'\0'};
	char line[LINE_DRT_FILE_LEN + 1] = {'\0'};
	char funcao[VALOR_FUNCAO_LEN + 1] = {'\0'};

	if((fUserIdDRT = fopen(userIdFullPath, "r")) == NULL){
		return(PAINEL_NOK);
	}

	while(!feof(fUserIdDRT)){
		memset(line, '\0', LINE_DRT_FILE_LEN + 1);

		if(fgets(line, LINE_DRT_FILE_LEN, fUserIdDRT) == NULL)
			break;

		if(line[0] == '#') /* Comment */
			continue;

		c = strchr(line, '-');
		if(c == NULL)
			continue; /* linha mal formatada, sem '-' */

		changeCharByChar(line, '\n', '\0');

		newNode = (userId_t *)malloc(sizeof(userId_t));
		if(newNode == NULL){
			fclose(fUserIdDRT);
			return(PAINEL_NOK);
		}

		memset(newNode,  0 , sizeof(userId_t)    );
		memset(drt,    '\0', DRT_LEN          + 1);
		memset(funcao, '\0', VALOR_FUNCAO_LEN + 1);

		c = line;
		cutter(&c, '-', drt, DRT_LEN);
		if(*c == '\0') return(PAINEL_NOK);

		cutter(&c, '\0', funcao, VALOR_FUNCAO_LEN);

		strncpy(newNode->userId, drt, DRT_LEN);
		newNode->level = string_2_UserType_t(funcao);

		if(ll_add(&head, newNode) == LL_NOK){
			fclose(fUserIdDRT);
			ll_destroyList(&head);
			return(PAINEL_NOK);
		}
	}

	fclose(fUserIdDRT);
	return(PAINEL_OK);
}

char * userType_t_2_String(tipoUsuario_t usrType)
{
	switch(usrType){
		case FORNO_ELETRICO:
			return(STR_FORNOELETRICO);

		case OPERADOR_MAQUINA:
			return(STR_OPERMAQUINA);

		case SUPERVISOR_MAQUINA:
			return(STR_SUPMAQUINA);

		case UNDEFINED_USER:
		default:
			return(NULL);
	}

	return(NULL);
}

tipoUsuario_t string_2_UserType_t(char *userStr)
{
	if(strcmp(STR_FORNOELETRICO, userStr) == 0)
		return(FORNO_ELETRICO);

	else if(strcmp(STR_OPERMAQUINA, userStr) == 0)
		return(OPERADOR_MAQUINA);

	else if(strcmp(STR_SUPMAQUINA, userStr) == 0)
		return(SUPERVISOR_MAQUINA);

	return(UNDEFINED_USER);
}
