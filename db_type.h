#include <stddef.h>
#include <stdbool.h>

struct data_base {
	struct Linklist *table[10];
	char *name;
};

struct data_set {
	char *key;
	char *value;
};
