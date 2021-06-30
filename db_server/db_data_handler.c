#include<sys/stat.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include"db_call_table.h"


struct Linklist* db_ds_list_p;

char* suc = "sucess";
char* fal = "false";
char* terminate = "\0";

char* check_NULL(char* msg)
{
  if(msg==NULL)
  {
    msg ="NULL";
  }
  return msg;
}

char *process_query_compress(struct query_result_ *qr_p)
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

char* process_call_table(char* buff, db_ds** db_ds_p_p)
{
  struct raw_data* rd_p = analysis(buff);
  char* target = rd_p->data[rd_p->index-1];
  char* opcode = rd_p->data[0];
  db_ds* db_ds_p = *db_ds_p_p;
  if(db_ds_p==NULL||(strcmp(db_ds_p->name,target)!=0))
    db_ds_p = pool_read(db_ds_list_p,target,3,NULL);
  if(strcmp(opcode,"create_db")==0)
  {
    db_ds_p = pool_read(db_ds_list_p,target,1,NULL);
    *db_ds_p_p=db_ds_p;
    if(db_ds_p!=NULL)
      return suc;
    else
      return fal;
  }
  else if(strcmp(opcode,"close_db")==0)
  {
    bool result = pool_remove(db_ds_list_p,target,NULL);
    if(result==true)
      return terminate;
    else
      return fal;
  }
  else if(strcmp(opcode,"open_db")==0)
  {
    db_ds_p = pool_read(db_ds_list_p,target,2,NULL);
    *db_ds_p_p=db_ds_p;
    if(db_ds_p!=NULL)
      return suc;
    else
      return fal;
  }
  else if(strcmp(opcode,"put")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not active db\n");
      return fal;
    }
    else
    {
      bool result = put(db_ds_p->db_p,rd_p->data[1],rd_p->data[2]);
      if(result==true)
        return suc;
      else
        return fal;
    }
  }
  else if(strcmp(opcode,"update")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not_active db\n");
      return fal;
    }
    else
    {
      int result = update(db_ds_p->db_p,rd_p->data[1],rd_p->data[2]);
      if(result==true)
        return suc;
      else
        return fal;
    }
  }
  else if(strcmp(opcode,"update_if")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not_active db\n");
      return fal;
    }
    else
    {
      int result = update_if(db_ds_p->db_p,rd_p->data[1],rd_p->data[2],rd_p->data[3]);
      if(result ==1)
        return suc;
      else
        return fal;
    } 
  }
  else if(strcmp(opcode,"remove_key")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not_active db\n");
      return fal;
    }
    else
    {
      bool result = remove_key(db_ds_p->db_p,rd_p->data[1]);
      if(result == true)
        return suc;
      else
        return fal;
    }
  }
  else if(strcmp(opcode,"query")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not_active db\n");
      return fal;
    }
    else
    {
      struct query_result_ * result = query(db_ds_p->db_p,rd_p->data[1]);
      return process_query_compress(result);
    }
  }
  else if(strcmp(opcode,"get")==0)
  {
    if(db_ds_p==NULL)
    {
      printf("not_active db\n");
      return fal;
    }
    else
      return check_NULL(get(db_ds_p->db_p, rd_p->data[1]));
  }
  delete_raw_data(rd_p);
  return NULL;
}
int main()
{
  db_ds_list_p=create_Linklist();
  printf("[----db_data_handler created----]\n");
  if(mkfifo("data_pipe",0777)==-1)
  {
    if(errno != EEXIST)
    {
      printf("could not create fifo pipe\n");
      return 1;
    }
  }

  if(mkfifo("recv_pipe",0777)==-1)
  {
    if(errno != EEXIST)
    {
      printf("could not create recv pipe\n");
      return 1;
    }
  }
  db_ds* db_ds_temp=NULL;
  int data_pipe, recv_pipe;
  int buff_len = 256;
  char* command_buff = malloc(buff_len);
  char* result;
  while(1)
  {
    //printf("reading.....\n");
    memset(command_buff,0x0,buff_len);
    //printf("handler:opening_data_pipe....\n");
    data_pipe = open("data_pipe",O_RDONLY);
    //printf("[handler: data_pipe_opened]\n");
    read(data_pipe, command_buff, buff_len-1);
    close(data_pipe);
    //printf("db_handler- readed: %s\n",command_buff);
    result = process_call_table(command_buff,&db_ds_temp);
    //printf("db_handler- writed: %s\n",result);
    recv_pipe = open("recv_pipe",O_WRONLY);
    //printf("recv_pipe_opened\n");
    write(recv_pipe, result, buff_len-1);
    close(recv_pipe);
  }

  close(data_pipe);
  return 0;
}

