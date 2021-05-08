
#include <stddef.h>
#include <stdbool.h>

typedef char* key_t;
typedef char* val_t;
typedef void* db_t;

// create and initialize a db
db_t create_db(char* name);

char* get_db_name(db_t db);

// Search the db using key
// return the value if exist otherwise return NULL
val_t get(db_t db, key_t key);

// Put the key/value pair into db
// return false if key exist, otherwise return true
bool put(db_t db, key_t key, val_t val);

// Remove the key/value pair from the db
// Return true if success, false otherwise
bool remove_key(db_t db, key_t key);

// Update the key/value pair from the db
// Return true if success, false otherwise
bool update(db_t db, key_t key, val_t val);

// Query result type
// store result in a link list
struct query_result_ {
  key_t key;
  struct query_result_* next;
};
typedef struct query_result_* query_result_t;

// Query the value from the db
// return keys using the above structure
query_result_t query(db_t db, val_t value);

// Delete query_result_t
void delete_query_result(query_result_t qr);

