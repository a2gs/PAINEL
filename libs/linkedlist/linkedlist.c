/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* linkedlist.c
 * A simple linkedlist assembled just for PAINEL.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 05/03/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdlib.h>

#include "linkedlist.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
int ll_create(ll_node_t **head)
{
	*head = NULL;
	return(LL_OK);
}

int ll_add(ll_node_t **head, void *data)
{
	ll_node_t *aux = NULL;
	ll_node_t *theEnd = NULL;

	aux = (ll_node_t *)malloc(sizeof(ll_node_t));
	if(aux == NULL)
		return(LL_NOK);

	if(*head == NULL){
		aux->next = NULL;
		aux->data = data;
		*head = aux;

		return(LL_OK);
	}

	for(theEnd = *head; theEnd->next != NULL; theEnd = theEnd->next);

	aux->next = NULL;
	aux->data = data;
	theEnd->next = aux;

	return(LL_OK);
}

int ll_delete(ll_node_t **head, ll_node_t *del, short doesFreeMem)
{
	ll_node_t *walker = NULL;

	if(*head == del){
		*head = del->next;
		free(del->data);
		free(del);
		return(LL_OK);
	}

	for(walker = *head; walker != NULL; walker = walker->next){
		if(walker->next == del){
			walker->next = del->next;
			if(doesFreeMem != 0)
				free(del->data);
			free(del);
			break;
		}
	}

	return(LL_OK);
}

int ll_destroyList(ll_node_t **head)
{
	ll_node_t *aux1 = NULL;
	ll_node_t *aux2 = NULL;

	for(aux1 = *head; aux1 != NULL; aux1 = aux2){
		aux2 = aux1->next;
		free(aux1->data);
		free(aux1);
	}

	*head = NULL;

	return(LL_OK);
}
