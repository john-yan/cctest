#include<stdio.h>
#include"Linklist.h"
#include<stdlib.h>
#include"db.h"
#include"db_internal.h"
#include"db_IO.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
char SUCESS[10] = "sucess";
char FALSE[10] = "false";
char ERROR_MSG_CANT_FIND_DB[40] = "Error: can't find active data base";

void send_suc(int client_socket)
{
	send(client_socket, SUCESS, sizeof(SUCESS), 0);
}

void send_fal(int client_socket)
{
	send(client_socket, FALSE, sizeof(FALSE), 0);
}

void send_err_msg_misdb(int client_socket)
{
	send(client_socket, ERROR_MSG_CANT_FIND_DB,
	     sizeof(ERROR_MSG_CANT_FIND_DB), 0);
}

void send_msg(char *msg, int client_socket)
{
	char send_buff[256];
	strcpy(send_buff, msg);
	send(client_socket, &send_buff, sizeof(send_buff), 0);
}

void server_delete_raw_data(struct raw_data *p)
{
	int index = p->index;
	for (int i = 0; i < index; i++) {
		free(p->data[i]);
	}
	free(p);
}

struct data_base *find_db(struct Linklist *db_list_p, char *target,
			  struct Linklist_node **return_node_p)
{
	struct Linklist_node *node_p = Linklist_get_start(db_list_p);
	struct data_base *db_p = (struct data_base *)Linklist_get_data(node_p);
	while (db_p != NULL) {
		if (strcmp(db_p->name, target) == 0) {
			*return_node_p = node_p;
			return db_p;
		}
		node_p = Linklist_next_node(node_p);
		db_p = (struct data_base *)Linklist_get_data(node_p);
	}
	return NULL;
}

char *query_compress(struct query_result_ *qr_p)
{
	int temp_length = 0;
	int string_length = 256;
	char *temp = malloc(string_length);
	memset(temp, 0x0, string_length);
	char *index_temp = malloc(16);
	int key_len;
	if (qr_p == NULL)
		strncpy(temp, "-1\0", 3);
	while (qr_p != NULL) {
		key_len = strlen(qr_p->key);
		sprintf(index_temp, "%d ", key_len);
		strcat(temp, index_temp);
		strcat(temp, qr_p->key);
		temp_length += (key_len + 1 + get_bit(key_len));
		qr_p = qr_p->next;
	}
	strcat(temp, "-1\0");
	if (temp_length >= string_length) {
		memset(temp, 0x0, string_length);
		temp = "-1 error return mesage too long( >256 bytes )";
	}
	delete_query_result(qr_p);
	free(index_temp);
	return temp;
}

bool check_data(struct raw_data *p, int segment_nb)
{
	if (p->index != segment_nb) {
		printf
		    ("false: no enough segment\nfilled segment: %d\nrequied segment number; %d\n",
		     p->index, segment_nb);
		return false;
	}
	for (int i = 0; i < segment_nb; i++) {
		if (p->data[i] == NULL) {
			printf("false: NULL pointer found in the segment\n ");
			return false;
		}
	}
	return true;
}

void call_table(int client_socket, struct Linklist *db_list_p, char *buff,
		bool *terminate_signal, struct Linklist_node **current_node_p)
{
	if (buff != NULL) {
		struct data_base *db_p =
		    (struct data_base *)Linklist_get_data(*current_node_p);
		struct raw_data *raw_data_p = analysis(buff);
		char *target_db = raw_data_p->data[raw_data_p->index - 1];
		if (db_p == NULL || strcmp(db_p->name, target_db) != 0) {
			db_p = find_db(db_list_p, target_db, current_node_p);
		}
		if (strcmp(raw_data_p->data[0], "put") == 0) {
			if (db_p == NULL) {
				send_msg("put false error:not active data base", client_socket);	//check is db exist
			} else {
				if (check_data(raw_data_p, 4) == false) {
					send_msg
					    ("false: missing or too many operan",
					     client_socket);
				}
				bool result =
				    put(db_p, raw_data_p->data[1],
					raw_data_p->data[2]);
				if (result == false)
					send_fal(client_socket);
				else
					send_suc(client_socket);
			}
		}
		//open_db
		else if (strcmp(raw_data_p->data[0], "open_db") == 0) {
			if (db_p == NULL) {
				db_p = readin_data(raw_data_p->data[1]);
				if (db_p == NULL)
					send_fal(client_socket);
				else {
					send_suc(client_socket);
					Linklist_insert(db_list_p, db_p);
				}
			} else
				send_msg("false: already open a data base",
					 client_socket);
		}
		//get
		else if (strcmp(raw_data_p->data[0], "get") == 0) {
			if (db_p == NULL) {
				send_msg("error, can't find active data base",
					 client_socket);
			} else {
				char *temp = get(db_p, raw_data_p->data[1]);
				if (temp == NULL) {
					send_msg("NULL", client_socket);
				} else {
					send_msg(temp, client_socket);
				}
			}
		}
		//remove_key
		else if (strcmp(raw_data_p->data[0], "remove_key") == 0) {
			if (db_p == NULL) {
				send_msg("error, can't find active data base",
					 client_socket);
			} else {
				bool result =
				    remove_key(db_p, raw_data_p->data[1]);
				if (result == true) {
					send_suc(client_socket);
				} else {
					send_fal(client_socket);
				}
			}
		}
		//query i
		else if (strcmp(raw_data_p->data[0], "query") == 0) {
			if (db_p == NULL) {
				send_err_msg_misdb(client_socket);
			} else {
				struct query_result_ *result_p =
				    (struct query_result_ *)query(db_p,
								  raw_data_p->
								  data[1]);
				char *temp = query_compress(result_p);
				delete_query_result(result_p);
				send_msg(temp, client_socket);
				free(temp);
			}
		}
		//close_db
		else if (strcmp(raw_data_p->data[0], "close_db") == 0) {
			if (db_p != NULL) {
				bool result = write_data(db_p);
				if (result == true) {
					send_msg("close_db sucess",
						 client_socket);
					Linklist_remove(db_list_p,
							*current_node_p);
					*terminate_signal = true;
				} else {
					send_msg("close_db flase",
						 client_socket);
				}
			} else {
				send_msg
				    ("close_db false error: can't find active data base",
				     client_socket);
			}
		}
		//update
		else if (strcmp(raw_data_p->data[0], "update") == 0) {
			if (db_p == NULL) {
				send_msg("error, can't find active data base",
					 client_socket);
			} else {
				bool result =
				    update(db_p, raw_data_p->data[1],
					   raw_data_p->data[2]);
				if (result == true) {
					send_suc(client_socket);
				} else
					send_fal(client_socket);
			}

		}
		//create_db
		else if (strcmp(raw_data_p->data[0], "create_db") == 0) {
			if (db_p == NULL) {
				send_suc(client_socket);
				db_p =
				    (struct data_base *)create_db(raw_data_p->
								  data[1]);
				Linklist_insert(db_list_p, (void *)db_p);
			} else {
				send_msg
				    ("create_db false error: already open a date base",
				     client_socket);
			}
		} else {
			send_msg("error: invalied command", client_socket);
		}
		server_delete_raw_data(raw_data_p);
	}
}

void client_fram(int client_socket)
{
	struct Linklist *db_list_p = create_Linklist();	// list of opened db
	struct Linklist_node *current_node_p = NULL;	// store recently accesed db
	int buff_length = 256;
	char *buff = malloc(buff_length);
	bool terminate_signal = false;
	memset(buff, 0x0, buff_length);
	while (terminate_signal == false) {
		recv(client_socket, buff, buff_length, 0);
		printf("recv:%s\n", buff);
		call_table(client_socket, db_list_p, buff, &terminate_signal,
			   &current_node_p);
		memset(buff, 0x0, buff_length);
	}
	delete_Linklist(db_list_p);
	free(buff);
	return;
}

int main()
{
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr *)&server_address,
	     sizeof(server_address));

	while (true) {
		listen(server_socket, 1024);
		int client_socket;
		client_socket = accept(server_socket, NULL, NULL);
		send_suc(client_socket);
		client_fram(client_socket);
		close(client_socket);
	}
	return 0;
}
