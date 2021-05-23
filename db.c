#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include "Linklist.h"
#include<stdlib.h>
typedef char *key_t_;
typedef char *val_t;
typedef void *db_t;

struct data_base {
	struct Linklist *table[10];
	char *name;
};

struct data_set {
	char *key;
	char *value;
};

struct query_result_ {
	key_t_ key;
	struct query_result_ *next;
};

typedef struct query_result_ *query_result_t;

struct query_result_ *new_query_result_node(char *key)
{
	struct query_result_ *new_p = malloc(sizeof(struct query_result_ *));
	new_p->key = key;
	return new_p;
}

struct query_result_ *query_result_insert(struct query_result_ *root, char *key)
{
	struct query_result_ *new_p = malloc(sizeof(struct query_result_ *));
	new_p->key = key;
	if (root != NULL)
		root->next = new_p;
	new_p->next = NULL;
	return new_p;
}

db_t create_db(char *name)
{
	struct data_base *new_data = malloc(sizeof(struct data_base));
	int i = 0;
	for (i; i < 10; i++) {
		new_data->table[i] = create_Linklist();
	}
	new_data->name = name;
	return new_data;
}

char *get_db_name(db_t db)
{
	struct data_base *dp = (struct data_base *)db;
	return dp->name;
}

int hashing(char *arr, int arr_size)
{
	int result = 0;
	int i = 0;
	while (i < arr_size) {
		result += arr[i];
		i++;
	}
	return result % 10;
}

void print_table(struct data_base *dp)
{
	printf("------------------\n");
	struct data_set *data_p;
	int i = 0;
	struct Linklist_node *node_p;
	for (i; i < 10; i++) {
		printf("%d: ", i);
		node_p = Linklist_get_start(dp->table[i]);
		while (node_p != NULL) {
			data_p = (struct data_set *)Linklist_get_data(node_p);
			node_p = Linklist_next_node(node_p);
			if (data_p != NULL)
				printf("key:%s value:%s-", data_p->key,
				       data_p->value);
			else
				printf("error!!\n");
		}
		printf("\n");
	}
	printf("------------------\n");
}

struct Linklist_node *search_key(struct Linklist_node *p, char *key)
{
	if (p == NULL)
		return NULL;
	else {
		struct data_set *data_p =
		    (struct data_set *)Linklist_get_data(p);
		if (strcmp(data_p->key, key) == 0)
			return p;
		else
			return search_key(Linklist_next_node(p), key);
	}
}

bool put(db_t db, key_t_ key, val_t val)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	if (search_key(Linklist_get_start(dp->table[index]), key) == NULL) {
		struct data_set *data_p = malloc(sizeof(struct data_set));
		data_p->key = malloc(1 + (strlen(key) * sizeof(char)));
		data_p->value = malloc(1 + (strlen(val) * sizeof(char)));
		strcpy(data_p->key, key);
		strcpy(data_p->value, val);
		Linklist_insert(dp->table[index], data_p);
		//print_table(dp);
		return true;
	} else
		return false;
}

bool remove_key(db_t db, key_t_ key)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	struct Linklist *target_Linklist = dp->table[index];
	struct Linklist_node *result =
	    search_key(Linklist_get_start(dp->table[index]), key);
	if (result == NULL) {
		return false;
	} else {
		struct data_set *data_p =
		    (struct data_set *)Linklist_get_data(result);
		free(data_p->key);
		free(data_p->value);
		Linklist_remove(target_Linklist, result);
		//  print_table(dp);
		return true;
	}
}

bool update(db_t db, key_t_ key, val_t val)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	struct Linklist_node *result =
	    search_key(Linklist_get_start(dp->table[index]), key);
	if (result == NULL) {
		return false;
	} else {
		struct data_set *data_p = Linklist_get_data(result);
		free(data_p->value);
		data_p->value = malloc(1 + (sizeof(strlen(val))));
		strcpy(data_p->value, val);
		return true;
	}
}

query_result_t query(db_t db, val_t value)
{
	struct data_base *dp = (struct data_base *)db;
	struct query_result_ *root = NULL;
	struct query_result_ *result = NULL;
	struct data_set *data_p;
	for (int i = 0; i < 10; i++) {
		for (struct Linklist_node * node_p =
		     Linklist_get_start(dp->table[i]); node_p != NULL;
		     node_p = Linklist_next_node(node_p)) {
			data_p = Linklist_get_data(node_p);
			if (strcmp(data_p->value, value) == 0) {
				result =
				    query_result_insert(result, data_p->key);
				if (root == NULL) {
					root = result;
				}
			}
		}
	}
	return root;
}

val_t get(db_t db, key_t_ key)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	struct Linklist_node *iterator =
	    search_key(Linklist_get_start(dp->table[index]), key);
	struct data_set *data_set_p = NULL;
	if (iterator >= 0) {
		data_set_p = (struct data_set *)Linklist_get_data(iterator);
	}
	if (data_set_p != NULL) {
		return data_set_p->value;
	} else
		return NULL;
}

void close_db(char *name)
{
}

db_t open_db(char *name)
{
	return NULL;
}

void delete_query_result(query_result_t qr)
{
	if (qr != NULL) {
		delete_query_result(qr->next);
		free(qr);
	} else
		return;
}
