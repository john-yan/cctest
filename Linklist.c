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

bool Linklist_remove(struct Linklist *list_p, int location)
{
	if (list_p != NULL) {
		struct Linklist_node *p = list_p->start;
		int i = 0;
		while (p != NULL && i < location) {
			p = p->next;
			i++;
		}
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

void *Linklist_get_data(struct Linklist *list_p, int location)
{
	int i = 0;
	struct Linklist_node *p = list_p->start;
	while (p != NULL && i < location) {
		p = p->next;
		i++;
	}
	if (p != NULL)
		return p->data;
	else
		return NULL;
}

int Linklist_get_length(struct Linklist *list_p)
{
	return list_p->size;
}
