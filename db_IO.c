#include <sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"Linklist.h"
#include"db.h"
#include"db_IO.h"
#include"db_internal.h"

#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

char *to_txt_file(char *name)
{
	int n = strlen(name);
	char na[n];
	char file[] = ".txt";
	strcpy(na, name);
	strcat(na, file);
	char *result = malloc(strlen(na));
	strcpy(result, na);
	return result;
}

struct raw_data *analysis(char *str_p)
{
	struct raw_data *rd = malloc(sizeof(struct raw_data));
	rd->index = 0;
	int cnt = 0;
	char *temp = malloc(strlen(str_p));
	memset(temp, 0x0, strlen(str_p));
	char *temp_p = temp;
	strcpy(temp, str_p);
	while (*str_p != '\0') {
		if (*str_p == ' ' || *str_p == '\n') {
			char *temp_save = malloc(cnt + 1);
			memset(temp_save, 0x0, cnt + 1);
			str_p++;
			strncpy(temp_save, temp, cnt);
			raw_data_insert(rd, temp_save);
			free(temp_save);
			temp += cnt + 1;
			cnt = 0;
		} else {
			cnt++;
			str_p++;
		}
	}
	free(temp_p);
	return rd;
}

int read_header(int fd)
{
	char *indicator = malloc(2);
	char *temp = malloc(10);
	memset(temp, 0x0, 10);
	memset(indicator, 0x0, 2);
	ssize_t flag;
	do {
		flag = read(fd, indicator, 1);
		strcat(temp, indicator);
	} while (*indicator != ' ' && *indicator != '\n' && flag > 0);
	int result = atoi(temp);
	free(temp);
	free(indicator);
	return result;
}

struct data_base *readin_data(char *name)
{
	char *file_name = to_txt_file(name);
	int fd = open(file_name, O_RDONLY, DEF_MODE);
	if (fd < 0) {
		close(fd);
		printf("can't find file\n");
		free(file_name);
		return NULL;
	} else if (fd >= 0) {
		struct data_base *data_base_p = create_db(name);
		int header = read_header(fd);
		while (header > 0) {
			char *temp = malloc(header);
			memset(temp, 0x0, header);
			read(fd, temp, header);
			printf("%s", temp);
			struct raw_data *raw_data_p = analysis(temp);
			int index = atoi(raw_data_p->data[0]);
			struct data_set *data_set_p =
			    malloc(sizeof(struct data_set));
			data_set_p->key = malloc(strlen(raw_data_p->data[1]));
			data_set_p->value = malloc(strlen(raw_data_p->data[2]));
			strcpy(data_set_p->key, raw_data_p->data[1]);
			strcpy(data_set_p->value, raw_data_p->data[2]);
			Linklist_insert(data_base_p->table[index], data_set_p);
			delete_raw_data(raw_data_p);
			free(temp);
			header = read_header(fd);
		}
		free(file_name);
		close(fd);
		return data_base_p;
	}
	return NULL;
}

int get_bit(int input)
{
	int result = 1;
	input = input / 10;
	while (input != 0) {
		input = input / 10;
		result++;
	}
	return result;
}

void write_data(struct data_base *dp)
{
	char *file_name = to_txt_file(dp->name);
	int fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, DEF_MODE);
	if (fd < 0) {
		printf("can't find file");
	} else {
		struct Linklist *list_p;
		struct data_set *data_set_p;
		struct Linklist_node *node_p;
		char *index_temp = malloc(20);
		memset(index_temp, 0x0, 20);
		int line_length;
		for (int i = 0; i < 10; i++) {
			list_p = dp->table[i];
			node_p = Linklist_get_start(list_p);
			while (node_p != NULL) {
				line_length = 0;
				data_set_p = (struct data_set *)
				    Linklist_get_data(node_p);
				int string_length =
				    strlen(data_set_p->key) +
				    strlen(data_set_p->value) + 5;
				char *temp = malloc(string_length);
				memset(temp, 0x0, string_length);
				sprintf(index_temp, "%d", string_length - 1);
				line_length += get_bit(string_length - 1);
				line_length += string_length;
				strcat(temp, index_temp);
				strcat(temp, " ");
				sprintf(index_temp, "%d", i);
				strcat(temp, index_temp);
				strcat(temp, " ");
				strcat(temp, data_set_p->key);
				strcat(temp, " ");
				strcat(temp, data_set_p->value);
				strcat(temp, "\n");
				write(fd, temp, line_length);
				free(temp);
				node_p = Linklist_next_node(node_p);
			}
		}
		sprintf(index_temp, "%d", -1);
		write(fd, index_temp, 2);
		free(index_temp);
		free(file_name);
		delete_data_base(dp);
		close(fd);
	}
}
