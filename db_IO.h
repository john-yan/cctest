#include <sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"Linklist.h"

#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

struct data_base *readin_data(char *name);

void write_data(struct data_base *db);
