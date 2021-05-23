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
	int i2 = 0;
	struct data_set *data_p;
	int size = 0;
	int i = 0;
	for (i; i < 10; i++) {
		printf("%d: ", i);
		i2 = 0;
		size = Linklist_get_length(dp->table[i]);
		for (i2; i2 < size; i2++) {
			data_p =
			    (struct data_set *)Linklist_get_data(dp->table[i],
								 i2);
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

int search_key(struct Linklist *list_p, char *key)
{
	int length = Linklist_get_length(list_p);
	int current = -1;
	struct data_set *data_p;
	bool match = false;
	int i = 0;
	for (i; i < length; i++) {
		data_p = (struct data_set *)Linklist_get_data(list_p, i);
		if (strcmp(data_p->key, key) == 0) {
			match = true;
			current = i;
			break;
		}
	}
	if (match == false)
		return -2;
	else
		return current;
}

bool put(db_t db, key_t_ key, val_t val)
{
	struct data_base *dp = (struct data_base *)db;
	int index = hashing(key, strlen(key));
	if (search_key(dp->table[index], key) < 0) {
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
	int result = search_key(target_Linklist, key);
	if (search_key(target_Linklist, key) < 0) {
		return false;
	} else {
		struct data_set *data_p =
		    (struct data_set *)Linklist_get_data(target_Linklist,
							 result);
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
	int result = search_key(dp->table[index], key);
	if (result < 0) {
		return false;
	} else {
		struct data_set *data_p =
		    Linklist_get_data(dp->table[index], result);
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
	int i = 0;
	for (i; i < 10; i++) {
		int i2 = 0;
		int length = Linklist_get_length(dp->table[i]);
		for (i2; i2 < length; i2++) {
			data_p = Linklist_get_data(dp->table[i], i2);
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
	int iterator = search_key(dp->table[index], key);
	struct data_set *data_set_p = NULL;
	if (iterator >= 0) {
		data_set_p =
		    (struct data_set *)Linklist_get_data(dp->table[index],
							 iterator);
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
