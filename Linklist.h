#include<stddef.h>
#include<stdbool.h>
struct Linklist_node;
struct Linklist;

bool Linklist_remove(struct Linklist *list_p, struct Linklist_node *node_p);

void Linklist_insert(struct Linklist *list_p, void *data);

struct Linklist *create_Linklist();

void *Linklist_get_data(struct Linklist_node *p);

int Linklist_get_length(struct Linklist *list_p);

struct Linklist_node *Linklist_next_node(struct Linklist_node *p);

struct Linklist_node *Linklist_get_start(struct Linklist *);

struct Linklist_node *Linklist_get_end(struct Linklist *);
