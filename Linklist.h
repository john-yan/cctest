#include<stddef.h>
#include<stdbool.h>
struct Linklist_node {
	struct Linklist_node *last;
	struct Linklist_node *next;
	void *data;
};

struct Linklist {
	struct Linklsit_node *start;
	struct Linklist_node *end;
	int size;
};

bool Linklist_remove(struct Linklist *list_p, int location);

void Linklist_insert(struct Linklist *list_p, void *data);

struct Linklist *create_Linklist();

void *Linklist_get_data(struct Linklist *list_p, int location);

int Linklist_get_length(struct Linklist *list_p);
