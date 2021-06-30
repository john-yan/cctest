#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
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

typedef struct query_result *query_result_t;

cd* cd_init(char* name)
{
  cd* cd_p = malloc(sizeof(cd));
  cd_p->name = malloc(strlen(name)+1);
  memset(cd_p->name,0x0,strlen(name)+1);
  strncpy(cd_p->name,name,strlen(name));
  cd_p->connection_status = -1;
  cd_p->network_socket = -1;
  return cd_p;
}

void delete_cd(cd* cd_p)
{
  free(cd_p->name);
  free(cd_p);
}
struct query_result *query_result_insert(struct query_result *root, char *key)
{
	struct query_result *new_p = malloc(sizeof(struct query_result));
	new_p->key = malloc(strlen(key) + 1);
	memset(new_p->key, 0x0, strlen(key) + 1);
	strncpy(new_p->key, key, strlen(key));
	if (root != NULL)
		root->next = new_p;
	new_p->next = NULL;
	return new_p;
}

void delete_query_result(query_result_t qr)
{
	if (qr != NULL) {
		delete_query_result(qr->next);
		free(qr->key);
		free(qr);
		return;
	} else
		return;
}

bool check_is_NULL(char *p)
{
	if (strcmp(p, "NULL") == 0)
		return true;
	else
		return false;
}

int get_size(char *str_p, int *header)
{
	char *temp = malloc(16);
	int index = 0;		//return number of readed bytes
	memset(temp, 0x0, 16);
	while ((*str_p) != ' ' && (*str_p) != '\0') {
		strncat(temp, str_p, 1);
		str_p++;
		index++;
	}
	*header = atoi(temp);
	free(temp);
	return index;
}

struct query_result *query_extract(char *str_p)
{
	if (str_p != NULL) {
		int header;
		int temp_length = 128;
		struct query_result *root = NULL;
		struct query_result *current_node = NULL;
		char *temp = malloc(temp_length);
		memset(temp, 0x0, 128);
		int num = get_size(str_p, &header);
		while (header != -1) {
			str_p += num + 1;
			memset(temp, 0x0, temp_length);
			strncpy(temp, str_p, header);
			current_node = query_result_insert(current_node, temp);
			if (root == NULL)
				root = current_node;
			str_p += header;
			num = get_size(str_p, &header);
		}
		free(temp);
		return root;
	}
	return NULL;
}

char *send_comand(char *opcode, char *arg1, char *arg2, char *arg3, char* arg4, int network_socket)
{
	int opcode_len = strlen(opcode);
	int arg1_len = strlen(arg1);
	int arg2_len = strlen(arg2);
	int arg3_len = strlen(arg3);
    int arg4_len = strlen(arg4);
	int response_length = 256;
	char *response = malloc(response_length);
	memset(response, 0x0, 256);
	char send_buff[256];
	char *temp = malloc(response_length);
	char *index_temp = malloc(16);
	memset(index_temp, 0x0, 16);
	memset(temp, 0x0, response_length);
	memset(send_buff, 0x0, sizeof(send_buff));
	sprintf(index_temp, "%d ", opcode_len);
	strcat(temp, index_temp);
	strcat(temp, opcode);
	sprintf(index_temp, "%d ", arg1_len);
	strcat(temp, index_temp);
	strcat(temp, arg1);
	if (arg2_len != 0) {
		sprintf(index_temp, "%d ", arg2_len);
		strcat(temp, index_temp);
		strcat(temp, arg2);
	}
	if (arg3_len != 0) {
		sprintf(index_temp, "%d ", arg3_len);
		strcat(temp, index_temp);
		strcat(temp, arg3);
	}
    if(arg4_len != 0)
    {
      sprintf(index_temp,"%d ",arg4_len);
      strcat(temp, index_temp);
      strcat(temp, arg4);
    }
	strncpy(send_buff, temp, (int)strlen(temp));
	send(network_socket, &send_buff, sizeof(send_buff), 0);
	recv(network_socket, response, response_length, 0);
	free(temp);
	free(index_temp);
	return response;
}

bool response_check(char *p)
{
	if (strcmp(p, "sucess") == 0)
		return true;
	else
		return false;
}

void connect_to_server(cd* cd_p)
{
	cd_p->network_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;
	cd_p->connection_status = connect(cd_p->network_socket, (struct sockaddr *)&server_address,sizeof(server_address));
	int response_length = 256;
	char *response = malloc(response_length);
	memset(response, 0x0, 256);
	if (cd_p->connection_status == -1)
		printf("connection false");
	else {
        send(cd_p->network_socket,"client_t",strlen("client_t"),0);
		recv(cd_p->network_socket, response, response_length, 0);
	}
	free(response);
}

db_t create_db(char *name)	// 
{
  cd* cd_p = cd_init(name);
  connect_to_server(cd_p);
  int network_socket = cd_p->network_socket;
  int connection_status = cd_p->connection_status;
	if (connection_status < 0)
		printf("false to connect the server\n");
	else {
		char *response = send_comand("create_db", name, "", "","", network_socket);
		if (response_check(response) == true) {
			free(response);
			return (void *)cd_p;
		} else {
			free(response);
			return NULL;
		}
	}
	return NULL;
}

void close_db(db_t db)
{
  cd* cd_p = (cd*)db;
  int connection_status = cd_p->connection_status;
  int network_socket = cd_p->network_socket;
  char* name = cd_p-> name;
	if (connection_status < 0)
		printf("not connection\n");
	else {
		char *response =
		    send_comand("close_db", name, "", "","", network_socket);
		free(response);
		close(network_socket);
	}
    delete_cd(cd_p);
}

db_t open_db(char *name)
{
  cd* cd_p = cd_init(name);
  connect_to_server(cd_p);
  int connection_status = cd_p->connection_status;
  int network_socket = cd_p -> network_socket;
  if (connection_status < 0)
    printf("fail to  connect server\n");
	else {
		char *response = send_comand("open_db", name, "", "","", network_socket);
		if (response_check(response) == true) {
			free(response);
			return cd_p;
		} else {
			free(response);
			return NULL;
		}
	}
	return NULL;
}

bool update_if(db_t db, char* key, char* old_val, char* new_val)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p->connection_status;
  int network_socket = cd_p -> network_socket;
  if (connection_status < 0)
    printf("not connection\n");
  else
  {
    char* response = send_comand("update_if",key,old_val,new_val,cd_p->name,network_socket);
    bool result = response_check(response);
    return result;
  }
  return false;

}

char *get_db_name(db_t db)
{
  cd* cd_p = (cd*) db;
  return cd_p ->name;
}

bool put(db_t db, key_t_ key, val_t val)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p-> connection_status;
  int network_socket = cd_p -> network_socket;
	if (connection_status < 0)
		printf("false to connect to server\n");
	else {
		char *response =
		    send_comand("put", key, val, cd_p->name,"", network_socket);
		bool result = response_check(response);
		free(response);
		return result;
	}
	return false;
}

bool remove_key(db_t db, key_t_ key)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p-> connection_status;
  int network_socket = cd_p -> network_socket;
	if (connection_status < 0)
		printf("false to connect to server\n");
	else {
		char *response =
		    send_comand("remove_key", key, cd_p->name, "","",
				network_socket);
		bool result = response_check(response);
		free(response);
		return result;
	}
	return false;
}

bool update(db_t db, key_t_ key, val_t val)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p-> connection_status;
  int network_socket = cd_p -> network_socket;
	if (connection_status < 0)
		printf("false to connect to server\n");
	else {
		char *response =
		    send_comand("update", key, val, cd_p->name,"", network_socket);
		bool result = response_check(response);
		free(response);
		return result;
	}
	return false;
}

query_result_t query(db_t db, val_t val)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p-> connection_status;
  int network_socket = cd_p -> network_socket;
	if (connection_status < 0)
		printf("false to connect to server\n");
	else {
		char *response =
		    send_comand("query", val, cd_p->name,"", "", network_socket);
		struct query_result *result_p = query_extract(response);
		free(response);
		return result_p;
	}
	return NULL;
}

char *get(db_t db, key_t_ key)
{
  cd* cd_p = (cd*) db;
  int connection_status = cd_p-> connection_status;
  int network_socket = cd_p -> network_socket;
	if (connection_status < 0)
		printf("false to connect to server\n");
	else {
		char *response =
		    send_comand("get", key, cd_p->name,"", "", network_socket);
		if (check_is_NULL(response) == true) {
			free(response);
			return NULL;
		} else
			return response;
	}
	return NULL;
}

void print_qr(struct query_result *p)
{
	while (p != NULL) {
		printf("%s - ", p->key);
		p = p->next;
	}
}
