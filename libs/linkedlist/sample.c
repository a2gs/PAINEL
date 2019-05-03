#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

int main(int argc, char *argv[])
{
	ll_node_t *head = NULL;
	ll_node_t *walker = NULL;
	char *aux = NULL;
	int i = 0;

	ll_create(&head);

	for(i = 0; i < 100; i++){
		aux = malloc(10);
		memset(aux, 0, 10);

		sprintf(aux, "%09d", i);

		ll_add(&head, aux);
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000053") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000054") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000056") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000057") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000050") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000099") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000000") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		if(strcmp(walker->data, "000000001") == 0){
			ll_delete(&head, walker);
			break;
		}
	}

	for(walker = head; walker != NULL; walker = walker->next){
		printf("[%s]\n", (char *)walker->data);
	}

	ll_destroyList(&head);

	return(0);
}
