#include<stddef.h>
#include<stdbool.h>
#include<stdlib.h>
#include"Linklist.h"

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

void delete_data_base(struct data_base *dp);

void delete_data_set(struct data_set *p);

void raw_data_insert(struct raw_data *data, char *str);

void delete_raw_data(struct raw_data *p);

void check_free(void *p);
