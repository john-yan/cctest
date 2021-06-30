#include"db_descriptor.h"
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<semaphore.h>
#include<stdbool.h>

char SUCESS[10] = "sucess";
char FALSE[10] = "false";
char ERROR_MSG_CANT_FIND_DB[40] = "Error: can't find active data base";

void send_msg(char *msg, int client_socket)
{
  int buff_len =256;
	char* send_buff = malloc(buff_len);
    memset(send_buff,0x0,256);
	strncpy(send_buff, msg,strlen(msg));
	send(client_socket, send_buff, buff_len, 0);
}

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

void send_bool(bool input,int client_socket)
{
  if(input==true)
    send_suc(client_socket);
  else
    send_fal(client_socket);
}

void send_string(char* input,int client_socket)
{
  if(input==NULL)
    send_msg("NULL",client_socket);
  else
    send_msg(input,client_socket);
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


void call_table(struct Linklist* db_ds_list_p, int client_socket,char* buff,bool *terminate_signal,db_ds** db_ds_p_p, sem_t* pool_mutex)
{
  int mode = 0; //0: thread mode 1: mutiplex mode
  if(db_ds_p_p==NULL)
    mode=1;
  struct raw_data* rd_p = analysis(buff);
  char* target = rd_p->data[rd_p->index-1];
  char* opcode = rd_p->data[0];
  db_ds* db_ds_p = NULL;
  struct data_base* db_p;
  if(mode==0)
  {
    db_ds_p= *db_ds_p_p;
  }
  else
  {
    db_ds_p = pool_read(db_ds_list_p,target,3,NULL);
  }
  if(db_ds_p!=NULL)
  {
    db_p = db_ds_p->db_p;
  }
  else
    db_p=NULL;
  if(strcmp(opcode,"create_db")==0)//create_db
  {
    if(mode==0)
      db_ds_p=pool_read(db_ds_list_p,target,1,pool_mutex);
    else
      db_ds_p=pool_read(db_ds_list_p,target,1,NULL);
    if(db_ds_p_p!=NULL)
    {
      *db_ds_p_p=db_ds_p;
    }
    db_p = db_ds_p->db_p;
    if(db_ds_p!=NULL)
      send_suc(client_socket);
    else
      send_fal(client_socket);
  }
  else if(strcmp(opcode,"close_db")==0)//close_db
  {
    bool result;
    if(mode==0)
      result = pool_remove(db_ds_list_p,target,pool_mutex);
    else
      result = pool_remove(db_ds_list_p,target,NULL);

    if(result==true)
    {
      send_suc(client_socket);
      *terminate_signal=true;
    }
    else
      send_fal(client_socket);
  }
  else if(strcmp(opcode,"open_db")==0)//opend_db
  {
    if(mode==0)
      db_ds_p=pool_read(db_ds_list_p,target,2,pool_mutex);
    else
      db_ds_p=pool_read(db_ds_list_p,target,2,NULL);
    if(db_ds_p_p!=NULL)
    {
      *db_ds_p_p=db_ds_p;
    }
    db_p = db_ds_p->db_p;
    if(db_p!=NULL)
      send_suc(client_socket);
    else
      send_fal(client_socket);
  }
  else if(strcmp(opcode,"put")==0)//put
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    {
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      //printf("put lock on\n");
      fflush(stdout);
      bool result = put(db_p,rd_p->data[1],rd_p->data[2]);
      //printf("-----------\n");
      //printf("put from client:%d \n",client_socket);
      //print_table(db_p);
      fflush(stdout);
      send_bool(result,client_socket);
      //printf("put lock off\n");
      fflush(stdout);
      if(mode==0)
        sem_post(db_ds_p->mutex);
    }
  }
  else if(strcmp(opcode,"get")==0)//get
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    {
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      char* result = get(db_p,rd_p->data[1]);
      send_string(result,client_socket);
      if(mode==0)
        sem_post(db_ds_p->mutex);
    }
  }
  else if(strcmp(opcode,"remove_key")==0)
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    {
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      //printf("----------\nremove_key_before\n");
      fflush(stdout);
      //print_table(db_p);
      bool result = remove_key(db_p,rd_p->data[1]);
      send_bool(result, client_socket);
      //printf("----------\nremove_key_after\n");
      //print_table(db_p);
      fflush(stdout);
      if(mode==0)
        sem_post(db_ds_p->mutex);
    }
  }
  else if(strcmp(opcode,"query")==0)
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    {
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      struct query_result_* result = query(db_p,rd_p->data[1]);
      char* temp = query_compress(result);
      send_msg(temp,client_socket);
      if(mode==0)
        sem_post(db_ds_p->mutex); 
    }
  }
  else if(strcmp(opcode,"update_if")==0)
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    { 
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      int result = update_if(db_p,rd_p->data[1],rd_p->data[2],rd_p->data[3]);
      //print_table(db_p);
      fflush(stdout);
      if(result==1)
        send_suc(client_socket);
      else
        send_fal(client_socket);
      if(mode==0)
        sem_post(db_ds_p->mutex);
    }
  }
  else if(strcmp(opcode,"update")==0)
  {
    if(db_p==NULL)
      send_err_msg_misdb(client_socket);
    else
    {
      if(mode==0)
        sem_wait(db_ds_p->mutex);
      int result = update(db_p,rd_p->data[1],rd_p->data[2]);
      if(result==true)
        send_suc(client_socket);
      else
        send_fal(client_socket);
      if(mode==0)
        sem_post(db_ds_p->mutex);
    }
  }
  delete_raw_data(rd_p);
}
