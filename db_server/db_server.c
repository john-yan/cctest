#define __USE_MISC
#define _KERNEL
#define _GNU_SOURCE
#include<sys/select.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<unistd.h>
#include"Linklist.h"
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<semaphore.h>
#include<pthread.h>
#include<sys/wait.h>
#include"db_call_table.h"

struct Linklist* db_ds_list_p;//descriptor pool
sem_t* pool_mutex;

int thread_count;
sem_t* tc_mutex;

void send_server_type(int client_socket, int type)
{
  int temp_len = 64;
  char* temp = malloc(temp_len);
  memset(temp,0x0,temp_len);
  sprintf(temp,"%d",type);
  send(client_socket,temp,temp_len,0);
}

void SHUT_DOWN_SERVER()
{
  int signal = 1;
  while(signal>0)
  {
    printf("waiting fot other threads to exit......\n");
    sleep(5);
    sem_wait(tc_mutex);
    signal = thread_count;
    sem_post(tc_mutex);
  }
  struct Linklist_node* node_p = Linklist_get_start(db_ds_list_p);
  db_ds* db_ds_p;
  while(node_p!=NULL)
  {
    db_ds_p = (db_ds*)Linklist_get_data(node_p);
    write_data(db_ds_p->db_p);
    delete_db_ds(db_ds_p);
    node_p = Linklist_next_node(node_p);
  }
  delete_Linklist(db_ds_list_p);
  sem_destroy(pool_mutex);
  sem_destroy(tc_mutex);
  free(pool_mutex);
  free(tc_mutex);
  printf("----server shout_down----\n");
  exit(0);
}

void print_rd(struct raw_data* rd_p)
{
  int end = rd_p->index;
  printf("------------\n");
  for(int i=0;i<end;i++)
  {
    printf("%d - %s\n",i+1,rd_p->data[i]);
  }
  printf("------------\n");
}

//====================================
//----       thread mode        ------
//====================================
//* start
void* client_frame(void* arg)
{
  sem_wait(tc_mutex);
  thread_count++;
  sem_post(tc_mutex);
    pthread_detach(pthread_self());
    int client_socket=*(int*)arg;
    free(arg);
    send_server_type(client_socket,0);
	int buff_length = 256;
	char *buff = malloc(buff_length);
	bool terminate_signal = false;
    db_ds* db_ds_p =NULL;
	memset(buff, 0x0, buff_length);
	while (terminate_signal == false) {
		recv(client_socket, buff, buff_length, 0);
		//printf("recv:%s from %d\n", buff,client_socket);
		call_table(db_ds_list_p,client_socket, buff, &terminate_signal, &db_ds_p,pool_mutex);
		memset(buff, 0x0, buff_length);
	}
	free(buff);
    close(client_socket);
  sem_wait(tc_mutex);
  thread_count--;
  sem_post(tc_mutex);
}

int server_socket_init()
{
  int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr *)&server_address,
	     sizeof(server_address));
    return server_socket;
}

void thread_mode()
{
    db_ds_list_p=create_Linklist();
    pool_mutex = malloc(sizeof(sem_t));
    tc_mutex= malloc(sizeof(sem_t));
    sem_init(pool_mutex,0,1);
    sem_init(tc_mutex,0,1);
    thread_count = 0;

	int server_socket, client_socket;
    server_socket = server_socket_init();
	listen(server_socket, 1024);
    int response_len = 256;
    char* response = malloc(response_len);
    memset(response,0x0,response_len);
	while (true) {
      pthread_t thread;
      //printf("waiting for connection.....\n");
		client_socket = accept(server_socket, NULL, NULL);
		recv(client_socket, response, response_len-1, 0);
        if(strcmp(response,"SHUT_DOWN_SERVER")==0)
        {
          SHUT_DOWN_SERVER();
          close(client_socket);
          free(response);
          break;
        }
        else
        {
        int* p_client = malloc(sizeof(int));
        *p_client = client_socket;
        pthread_create(&thread,NULL,client_frame,p_client);
        }
	}
}
//====================================
//*end 
//====================================


//====================================
//-----      process mode        -----
//====================================
//* start
bool check_result(char* result,int client_socket)
{
  if(strcmp(result,"\0")==0)
  {
    send_suc(client_socket);
    return true;
  }
  else
  {
    send_msg(result,client_socket);
    return false;
  }
}
void fd_pool_insert(struct data_base* db_p, int pid, int fd)
{
  if(pid>0)
  {
  int buff_len =64;
  char* pid_temp = malloc(buff_len);
  char* fd_temp = malloc(buff_len);
  memset(pid_temp,0x0,buff_len);
  memset(fd_temp,0x0,buff_len);
  sprintf(pid_temp,"%d",pid);
  sprintf(fd_temp,"%d",fd);
  bool result = put(db_p,pid_temp,fd_temp);
  if(result == false)
  {
    printf("pool_inser fail\n");
    exit(1);
  }
  else
  {
    //printf("[* in pool: pid:%s fd:%s *]\n",pid_temp, fd_temp);
  }
  free(pid_temp);
  free(fd_temp);
  }
}

void fd_pool_recycle(struct data_base* db_p, int pid)
{
  if(pid>0)
  {
  int buff_len =64;
  char* pid_temp = malloc(buff_len);
  memset(pid_temp,0x0,buff_len);
  sprintf(pid_temp,"%d",pid);
  char* result = get(db_p,pid_temp);
  if(result==NULL)
  {
    printf("pid:%s missing\n",pid_temp);
    exit(1);
  }
  else
  {
    //printf("[* remove_from_pool: pid: %s fd:%s *]\n",pid_temp,result);
  }
  int fd = atoi(result);
  close(fd);
  //printf("[* close fd: %d *]\n",fd);
  remove_key(db_p,pid_temp);
  free(pid_temp);
  }
}

void process_mode()
{
  struct data_base* fd_pool = create_db("fd_pool");
  int parent_pid = getpid();
  if(mkfifo("data_pipe",0777)==-1)
  {
    if(errno != EEXIST)
    {
      printf("could not create fifo pipe\n");
      exit(1);
    }
  }
  if(mkfifo("recv_pipe",0777)==-1)
  {
    if(errno!=EEXIST)
    {
      printf("could not create recv pipe\n");
      exit(1);
    }
  }
  int server_socket, client_socket;
  server_socket = server_socket_init();
  listen(server_socket,1024);
  int buff_length = 256;
  char* buff = malloc(buff_length);
  int pid = parent_pid;
  int db_data_handler_id = fork();
  if(db_data_handler_id==0)
  {
    //exec db_data_handler
    char* args[] ={"./db_data_handler",NULL};
    execv(args[0],args);
  }
  sem_t * process_mutex = mmap(NULL, sizeof(sem_t),PROT_READ| PROT_WRITE, MAP_SHARED| 0x20, -1 ,0);
  sem_init(process_mutex,1,1);
  while(true)
  {
    if(pid!=0)
    {
      int rec_pid;
      //parent_process
      //printf("in parent\n");
	  client_socket = accept(server_socket, NULL, NULL);
      //printf("form parent, client_socket%d\n",client_socket);
      //fflush(stdout);
      memset(buff,0x0,buff_length);
      recv(client_socket,buff,buff_length,0);
      //printf("connection type: %s\n",buff);
      pid = fork();
      fd_pool_insert(fd_pool,pid,client_socket);
      rec_pid = waitpid(-1,NULL,WNOHANG);
      //printf(" printf:rec_pid: %d\n",rec_pid);
      while(rec_pid > 0)
      {
        fd_pool_recycle(fd_pool,rec_pid);
        rec_pid=waitpid(-1,NULL,WNOHANG);
        //printf(" printf:rec_pid: %d\n",rec_pid);
      }
      //fflush(stdout);
    }
    else
    {
      int buff_len = 256;
      char* buff = malloc(buff_len);
      send_server_type(client_socket,2);
      bool terminate_signal = false;
      int recv_pipe, data_pipe;
      while(terminate_signal==false)
      {
        sem_wait(process_mutex);
        //printf("[from child: client_socket%d]  ---lock on\n ",client_socket);
        //fflush(stdout);
        memset(buff,0x0,buff_len);
        recv(client_socket,buff,buff_len,0);
        data_pipe = open("data_pipe",O_WRONLY);
        //printf("[child: data_piep_opened]\n");
        write(data_pipe, buff, buff_len);
        close(data_pipe);
        //printf("write -client_socket: %d\n",client_socket);
        //fflush(stdout);
        memset(buff,0x0,buff_len);
        recv_pipe = open("recv_pipe",O_RDONLY);
        read(recv_pipe, buff, buff_len-1);
        close(recv_pipe);
        //printf("result_buff: %s\n",buff);
        //printf("[from child: client_socket%d]  ---lock off\n ",client_socket);
        //fflush(stdout);
        terminate_signal = check_result(buff,client_socket);
        sem_post(process_mutex);
      }
      close(client_socket);
      //printf("process exit %d\n:",getpid());
      exit(0);
      break;
    }

  }
}
//====================================
//*end 
//====================================



//====================================
//-----    I/O mutiplex mode    ------
//====================================

void connection_handler(int client_socket, fd_set* current_set, char* buff, int buff_length)// IO_mutiplex only
{
  memset(buff,0x0,buff_length);
  bool terminate_signal=false;
  recv(client_socket,buff,buff_length,0);
  //printf("recv from client:%s\n",buff);
  call_table(db_ds_list_p,client_socket,buff,&terminate_signal,NULL,NULL);
  if(terminate_signal==true)
  {
    FD_CLR(client_socket,current_set);
    close(client_socket);
    thread_count--;
  }
}
void mutiplex_mode()
{
    printf("IO mutiplexing mode\n");
    db_ds_list_p=create_Linklist();
    thread_count =0;

	int server_socket, client_socket;
    //init basic enviroment;
    server_socket = server_socket_init();

	listen(server_socket, 1024);

   fd_set current_set , ready_set;
   FD_ZERO(&current_set);
   FD_SET(server_socket,&current_set);
   int max_fd_num = server_socket;

   int buff_length = 256;
   char* buff = malloc(buff_length);

   while(true)
   {
     ready_set = current_set;
     if(select(FD_SETSIZE,&ready_set,NULL,NULL,NULL)<0)
     {
       perror("elect_error");
     }
     else
     {
       for ( int i=0;i<= max_fd_num ; i++)
       {
         if(FD_ISSET(i,&ready_set))
         {
           if(i == server_socket)
           {
             //new connection
		     client_socket = accept(server_socket, NULL, NULL);
             FD_SET(client_socket,&current_set);
             memset(buff,0x0,buff_length);
             recv(client_socket,buff,buff_length,0);
            // printf("connection type: %s\n",buff);
             send_server_type(client_socket,1);
             thread_count++;
             if(client_socket > max_fd_num)
             {
               max_fd_num = client_socket;
             }
           }
           else
           {
             connection_handler(i,&current_set,buff,buff_length);
           }

         }
       }
     }
   }

//====================================
//*end 
//====================================

}

int main(int argc, char* argv[])
{
  if(argc<2)
  {
    printf("missing init_option\n");
    exit(1);
  }
  char* init_option = argv[1];
  if(strcmp("1",init_option)==0)
  {
    printf("thread_mode\n");
    thread_mode();
  }
  else if(strcmp("2",init_option)==0)
  {
    printf("process_mode\n");
    process_mode();
  }
  else if(strcmp("3",init_option)==0)
  {
    mutiplex_mode();
  }
  else
  {
    printf("can't find option\n");
  }
	return 0;
}
