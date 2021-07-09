#include<stdbool.h>

typedef char *key_t_;
typedef char *val_t;
typedef void *db_t;

struct query_result {
	char *key;
	struct query_result *next;
};

typedef struct connection_descriptor
{
  char* name;
  int connection_status;
  int network_socket;
} cd;

db_t create_db(char *name);

db_t open_db(char* name);

bool put(db_t db, key_t_ key, val_t val);

char* get(db_t db, key_t_ key);

bool update(db_t db, key_t_ key, val_t val);

bool update_if(db_t db, key_t_ key, val_t old_val, val_t new_val);

void close_db(db_t db);

bool query(db_t db, val_t val);

void delete_query_result(struct query_result*);

bool remove_key(db_t db, key_t_ key);
