#include<string.h>
#include<stddef.h>
#include"Linklist.h"
#include<stdlib.h>

struct data_base {
	struct Linklist *table[10];
	char *name;
};

struct data_set {
	char *key;
	char *value;
};
struct raw_data {
	int index;
	char *data[3];
};

void raw_data_insert(struct raw_data *data, char *str)
{
	char *strp = malloc(strlen(str));
	strcpy(strp, str);
	data->data[data->index] = strp;
	data->index++;
}

void delete_data_set(struct data_set *p)
{
	free(p->key);
	free(p->value);
	free(p);
}

void delete_data_base(struct data_base *dp)
{
	free(dp->name);
	struct data_set *data_set_p;
	struct Linklist_node *node_p;
	for (int i = 0; i < 10; i++) {
		node_p = Linklist_get_start(dp->table[i]);
		while (node_p != NULL) {
			data_set_p = Linklist_get_data(node_p);
			delete_data_set(data_set_p);
			node_p = Linklist_next_node(node_p);
		}
		delete_Linklist(dp->table[i]);
	}
	free(dp);
}

void delete_raw_data(struct raw_data *p)
{
	int index = p->index;
	for (int i = 0; i < index; i++) {
		free(p->data[i]);
	}
	free(p);
}
