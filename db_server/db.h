#include <stddef.h>
#include <stdbool.h>

typedef char *key_t_;
typedef char *val_t;
typedef void *db_t;

struct data_base;

struct data_set;

db_t create_db(char *name);

db_t open_db(char *name);

void close_db(db_t db);

char *get_db_name(db_t db);

val_t get(db_t db, key_t_ key);

bool put(db_t db, key_t_ key, val_t val);

bool remove_key(db_t db, key_t_ key);

bool update(db_t db, key_t_ key, val_t val);

struct query_result_ {
	key_t_ key;
	struct query_result_ *next;
};

typedef struct query_result_ *query_result_t;

query_result_t query(db_t db, val_t value);

void delete_query_result(query_result_t qr);

void delete_data_base(struct data_base *dp);

int hashing(char *arr, int arr_size);

void print_table(struct data_base* db_p);

int update_if(db_t , key_t_ key, val_t old_val, val_t new_val);
