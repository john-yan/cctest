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

int get_size(char* str_p, int* header)
{
  char* temp = malloc(16);
  int index=0;//return number of readed bytes
  memset(temp,0x0,16);
  while((*str_p)!=' ')
  {
    strncat(temp,str_p,1);
    str_p++;
    index++;
  }
  *header = atoi(temp);
  free(temp);
  return index;
}


struct raw_data* analysis(char* str_p)
{
  struct raw_data* rd_p = malloc(sizeof(struct raw_data));
  rd_p->index=0;
  int header;//length of the data segment
  int num;
  while(*str_p!='\n')
  {
    num = get_size(str_p,&header);
    str_p+=(num+1);//move pinter num bytes to the left;
    char* temp = malloc(header);
    memset(temp,0x0,header);
    strncpy(temp,str_p,header);
    rd_p->data[rd_p->index]=malloc(header);
    strncpy(rd_p->data[rd_p->index],temp,header);
    rd_p->index++;
    free(temp);
    str_p+=header;
  }
  return rd_p;
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
			int index = hashing(raw_data_p->data[0],strlen(raw_data_p->data[0]));
			struct data_set *data_set_p =
			    malloc(sizeof(struct data_set));
			data_set_p->key = malloc(strlen(raw_data_p->data[0]));
			data_set_p->value = malloc(strlen(raw_data_p->data[1]));
			strncpy(data_set_p->key, raw_data_p->data[0],strlen(raw_data_p->data[0]));
			strncpy(data_set_p->value, raw_data_p->data[1],strlen(raw_data_p->data[1]));
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

bool write_data(struct data_base *dp)
{
	char *file_name = to_txt_file(dp->name);
	int fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, DEF_MODE);
	if (fd < 0) {
		printf("can't find file");
        free(file_name);
        return false;
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
				int string_length = strlen(data_set_p->key) +strlen(data_set_p->value) + 4; //4: 3 speace 1 \n
                string_length +=get_bit(strlen(data_set_p->key)+get_bit(strlen(data_set_p->value))); // add length of data header;
				line_length += (get_bit(string_length)+1);//add length of string header
				line_length += string_length;
				char *temp = malloc(line_length);
				memset(temp, 0x0, line_length);
                sprintf(temp,"%d %d %s%d %s\n",string_length,(int)strlen(data_set_p->key), data_set_p->key,(int)strlen(data_set_p->value),data_set_p->value);
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
        return true;
	}
}
