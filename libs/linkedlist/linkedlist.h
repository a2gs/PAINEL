#define LL_OK   (0)
#define LL_NOK  (-1)

typedef struct _ll_node_t{
	struct _ll_node_t *next;
	void *data;
}ll_node_t;

int ll_create(ll_node_t **head);
int ll_add(ll_node_t **head, void *data);
int ll_delete(ll_node_t **head, ll_node_t *del);
int ll_destroyList(ll_node_t **head);
