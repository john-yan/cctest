#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

struct Linklist_node {
	void *data;
	struct Linklist_node *next;
	struct Linklist_node *last;
};

struct Linklist {
	struct Linklist_node *start;
	struct Linklist_node *end;
	int size;
};

struct Linklist *create_Linklist()
{
	struct Linklist *p = malloc(sizeof(struct Linklist));
	p->start = NULL;
	p->end = NULL;
	p->size = 0;
	return p;
}

struct Linklist_node *new_Linklist_node(struct Linklist_node *last_node,
					void *data)
{
	struct Linklist_node *new_node = malloc(sizeof(struct Linklist_node));
	new_node->next = NULL;
	new_node->last = last_node;
	new_node->data = data;
	return new_node;
}

void Linklist_insert(struct Linklist *list_p, void *data)
{
	struct Linklist_node *p;
	p = new_Linklist_node(list_p->end, data);
	if (list_p->end != NULL)
		list_p->end->next = p;
	list_p->end = p;
	if (list_p->size == 0)
		list_p->start = list_p->end;
	list_p->size++;
}

bool Linklist_remove(struct Linklist *list_p, struct Linklist_node *node_p)
{
	if (list_p != NULL) {
		struct Linklist_node *p = node_p;
		if (p != NULL) {
			if (p->last != NULL && p->next != NULL) {
				p->last->next = p->next;
				p->next->last = p->last;
			} else if (p->last != NULL && p->next == NULL) {
				list_p->end = p->last;
				p->last->next = NULL;
			} else if (p->last == NULL && p->next != NULL) {
				list_p->start = p->next;
				p->next->last = NULL;
			} else if (p->last == NULL && p->next == NULL) {
				list_p->start = NULL;
				list_p->end = NULL;
			}
			free(p->data);
			free(p);
			list_p->size--;
			return true;
		} else {
			return false;
		}
	}
	return false;
}

void *Linklist_get_data(struct Linklist_node *p)
{
	if (p != NULL)
		return p->data;
	else
		return NULL;
}

int Linklist_get_length(struct Linklist *list_p)
{
	return list_p->size;
}

struct Linklist_node *Linklist_next_node(struct Linklist_node *p)
{
	if (p == NULL)
		return NULL;
	else
		return p->next;
}

struct Linklist_node *Linklist_get_start(struct Linklist *list_p)
{
	if (list_p != NULL)
		return list_p->start;
	else
		return NULL;
}

struct Linklist_node *Linklist_get_end(struct Linklist *list_p)
{
	if (list_p != NULL)
		return list_p->end;
	else
		return NULL;
}
