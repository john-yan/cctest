#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include "Linklist.h"
#include<stdlib.h>
#include"db_IO.h"
#include"db_internal.h"
typedef char *key_t_;
typedef char *val_t;
typedef void *db_t;

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
	for (int i = 0; i < 10; i++) {
		new_data->table[i] = create_Linklist();
	}
	new_data->name = malloc(strlen(name) + 1);
	memset(new_data->name, 0x0, strlen(name) + 1);
	strncpy(new_data->name, name, strlen(name));
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
		data_p->key = malloc(1 + strlen(key));
        memset(data_p->key,0x0,strlen(key)+1);
		data_p->value = malloc(1 + strlen(val));
        memset(data_p->value,0x0,strlen(val)+1);
		strncpy(data_p->key, key,strlen(key));
		strncpy(data_p->value, val,strlen(val));
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
		delete_data_set(data_p);
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
		data_p->value = malloc(1 + strlen(val));
        memset(data_p->value,0x0,strlen(val)+1);
		strncpy(data_p->value,val, strlen(val));
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

void close_db(db_t db)
{
	struct data_base *dp = (struct data_base *)db;
	write_data(dp);
}

int update_if(db_t db,char* key,char* old_val, char* new_val)
{
  struct data_base* db_p = (struct data_base*) db;
  struct Linklist_node* node_p = Linklist_get_start(db_p->table[hashing(key,strlen(key))]);
  node_p = search_key(node_p,key);
  if(node_p==NULL)
    return -1;// error code -1: can't find key
  else
  {
    struct data_set* data_set_p = (struct data_set*)Linklist_get_data(node_p);
    if(strcmp(data_set_p->value,old_val)==0)
    {
      free(data_set_p->value);
      data_set_p->value=malloc(strlen(new_val)+1);
      memset(data_set_p->value,0x0,strlen(new_val)+1);
      strncpy(data_set_p->value,new_val,strlen(new_val));
      return 1;
    }
    else
      return 0;
  }
}

db_t open_db(char *name)
{
	struct data_base *dp = readin_data(name);
    return dp;
}

void delete_query_result(query_result_t qr)
{
	if (qr != NULL) {
		delete_query_result(qr->next);
		free(qr);
	} else
		return;
}
