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
/*
 *
 * <Description>
 *
 * INPUT:
 * OUTPUT:
 */
int loadUserIdFileToMemory(ll_node_t **head, char *userIdFullPath)
{
	FILE *fUserIdDRT = NULL;
	char line[LINE_DRT_FILE_LEN + 1] = {'\0'};
	char *c = NULL;
	unsigned int i = 0;
	userId_t *newNode = NULL;
	char dtr[DRT_LEN + 1] = {'\0'};
	char funcao[VALOR_FUNCAO_LEN + 1] = {'\0'};

	if((fUserIdDRT = fopen(userIdFullPath, "r")) == NULL){
		return(PAINEL_NOK);
	}

	for(i = 0; !feof(fUserIdDRT); i++){
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

		memset(newNode, 0, sizeof(userId_t));

		c = line;
		cutter(&c, '-', dtr, DRT_LEN);
		if(*c == '\0') return(PAINEL_NOK);

		cutter(&c, '\0', funcao, VALOR_FUNCAO_LEN);
		if(*c == '\0') return(PAINEL_NOK);

		strncpy(newNode->userId, dtr, DRT_LEN);
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
