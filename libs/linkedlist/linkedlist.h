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


#ifndef __LLIST_H__
#define __LLIST_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */
#define LL_OK   (0)
#define LL_NOK  (-1)


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef struct _ll_node_t{
	struct _ll_node_t *next;
	void *data;
}ll_node_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* int ll_create(ll_node_t **head)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int ll_create(ll_node_t **head);

/* int ll_add(ll_node_t **head, void *data)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int ll_add(ll_node_t **head, void *data);

/* int ll_delete(ll_node_t **head, ll_node_t *del)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int ll_delete(ll_node_t **head, ll_node_t *del);

/* int ll_destroyList(ll_node_t **head)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int ll_destroyList(ll_node_t **head);

#endif
