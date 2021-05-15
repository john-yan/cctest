#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
typedef char *key_t;
typedef char *val_t;
typedef void *db_t;

struct Linklist_node {
	char value[20];
	char key[20];
	struct Linklist_node *next;
	struct Linklist_node *last;
};
struct query_result_ {
	key_t key;
	struct query_result_ *next;
};
typedef struct query_result_ *query_result_t;
struct query_result_ *new_query_result_node(char *tkey)
{
	struct query_result_ *new_p = malloc(sizeof(struct query_result_));
	new_p->key = tkey;
	return new_p;
}

struct query_result_ *query_result_insert(struct query_result_ *p, char *tkey)
{
	struct query_result_ *new_p = new_query_result_node(tkey);
	if (p != NULL)
		p->next = new_p;
	return new_p;
}

struct Linklist_node *new_Linklist_node(struct Linklist_node *last_node,
					char value[], char key[])
{
	struct Linklist_node *p;
	p = malloc(sizeof(struct Linklist_node));
	strcpy(p->value, value);
	strcpy(p->key, key);
	p->next = NULL;
	p->last = last_node;
	return p;
}

struct Linklist_node *Linklist_insert(struct Linklist_node *root, char value[],
				      char key[])
{
	struct Linklist_node *p = new_Linklist_node(root, value, key);
	root->next = p;
	return NULL;
}

struct Linklist_node *Linklist_search_key(struct Linklist_node *root,
					  char wanted_key[])
{
	while (root != NULL) {
		if (strcmp(root->key, wanted_key) == 0) {
			return root;
		}
		root = root->next;
	}
	return NULL;
}

struct Linklist_node *Linklist_search_value(struct Linklist_node *root,
					    char wanted_value[])
{
	while (root != NULL) {
		if (strcmp(root->value, wanted_value) == 0) {
			return root;
		}
		root = root->next;
	}
	return NULL;
}

struct data_base {
	struct Linklist_node *table[10];
	char name[20];
};

// create and initialize a db
db_t create_db(char *name)
{
	struct data_base *new_data = malloc(sizeof(struct data_base));
	int i = 0;
	for (int i = 0; i < 10; i++) {
		new_data->table[i] = NULL;
	}
	strcpy(new_data->name, name);
	return new_data;
}

char *get_db_name(db_t db)
{
	struct data_base *p = (struct data_base *)db;
	return p->name;
}

int hashing(char arr[], int arr_size)
{
	int result = 0;
	int i = 0;
	while (i < arr_size) {
		result += arr[i];
		i++;
	}
	return result % 10;
}

db_t open_db(char *name)
{
	// Please implement this
	return NULL;
}

void close_db(db_t db)
{
	// Please implement this
}

void print_linklist(struct Linklist_node *p)
{
	while (p != NULL) {
		printf("%s:%s-", p->key, p->value);
		p = p->next;
	}
	printf("\n");
}

void print_table(struct data_base *dp)
{
	int i = 0;
	printf("-------------\n");
	for (i; i < 10; i++) {
		printf("%d: ", i);
		print_linklist(dp->table[i]);
	}
	printf("--------------\n");
}

val_t get(db_t db, key_t key)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	if (dp->table[index] != NULL) {
		struct Linklist_node *p = dp->table[index];
		do {
			if (strcmp(p->key, key) == 0) {
				return p->value;
			} else {
				p = p->next;
			}
		} while (p != NULL);
	}
	return NULL;
}

bool put(db_t db, key_t key, val_t val)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	if (dp->table[index] == NULL) {
		dp->table[index] = new_Linklist_node(NULL, val, key);
	} else {
		struct Linklist_node *p = dp->table[index];
		do {
			if (strcmp(p->key, key) == 0) {
				return false;
			} else if (p->next == NULL)
				break;
			else
				p = p->next;
		} while (p != NULL);
		Linklist_insert(p, val, key);
	}
	//print_table(dp);
	return true;
}

bool remove_key(db_t db, key_t key)
{
	int index = hashing(key, strlen(key));
	struct data_base *dp = (struct data_base *)db;
	struct Linklist_node *p = Linklist_search_key(dp->table[index], key);
	if (p != NULL) {
		if ((p->last == NULL) && (p->next == NULL)) {
			dp->table[index] = NULL;
		} else if (p->next != NULL && p->last == NULL) {
			dp->table[index] = p->next;
			p->next->last = NULL;
		} else if (p->last != NULL && p->next == NULL) {
			p->last->next = NULL;
		} else {
			p->last->next = p->next;
			p->next->last = p->last;
		}
		free(p);
		return true;
		// print_table(dp);
	}
	return false;
}

// Update the key/value pair from the db
// Return true if success, false otherwise
bool update(db_t db, key_t key, val_t val)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	struct Linklist_node *p = Linklist_search_key(dp->table[index], key);
	if (p != NULL) {
		strcpy(p->value, val);
		return true;
	}
	return false;
}

query_result_t query(db_t db, val_t value)
{
	struct data_base *dp = (struct data_base *)db;
	struct query_result_ *root = NULL;
	struct query_result_ *result = NULL;
	int i = 0;
	struct Linklist_node *p;
	for (i; i < 10; i++) {
		p = Linklist_search_value(dp->table[i], value);
		while (p != NULL) {
			result = query_result_insert(result, p->key);
			if (root == NULL) {
				root = result;
			}
			p = Linklist_search_value(p->next, value);
		}

	}
	return root;
}

void delete_query_result(query_result_t qr)
{
	if (qr != NULL) {
		delete_query_result(qr->next);
		free(qr);
	} else
		return;
}
