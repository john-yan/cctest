#include"db.h"
#include"db_IO.h"
#include"db_internal.h"
#include"Linklist.h"
#include<semaphore.h>

typedef struct db_descriptor
{
  int flag;
  struct data_base* db_p;
  sem_t* mutex;
  char* name;
}db_ds;

void delete_db_ds(db_ds* p)
{
  if(p->mutex !=NULL)
  {
    sem_destroy(p->mutex);
    free(p->mutex);
  }
  free(p->name);
  free(p);
}

db_ds* db_ds_init(struct data_base* db_p, int mode)
  {
    db_ds* db_ds_p = malloc(sizeof(db_ds));
    db_ds_p->flag=1;
    db_ds_p->db_p = db_p;
    if(mode==0)
    {
      db_ds_p->mutex = malloc(sizeof(sem_t));
      sem_init(db_ds_p->mutex, 0,1);
      //initilize semaphore if mode is 0
    }
    else
      db_ds_p->mutex = NULL;
    db_ds_p->name = malloc(strlen(db_p->name)+1);
    memset(db_ds_p->name,0x0,strlen(db_p->name)+1);
    strncpy(db_ds_p->name,db_p->name,strlen(db_p->name));
    return db_ds_p;
  }

db_ds* pool_read_thread_mode(struct Linklist* db_ds_list_p, char* target_name, int opcode, sem_t* pool_mutex)
{
  sem_wait(pool_mutex);
  //printf("thread_mode- read- lock on\n");
  fflush(stdout);
  struct Linklist_node* node_p = Linklist_get_start(db_ds_list_p);
  db_ds* db_ds_p = NULL;
  while(node_p!=NULL)
  {
    db_ds_p = (db_ds*) Linklist_get_data(node_p);
    if(strcmp(target_name,db_ds_p->name)==0)
    {
      if(opcode<=2)
      {
        db_ds_p->flag++;
      }
      sem_post(pool_mutex);
      //printf("thread_mode- read- lock_off\n");
      fflush(stdout);
      return db_ds_p;
    }
    node_p=Linklist_next_node(node_p);
  }
  struct data_base* db_p;
  db_ds_p=NULL;
  if(opcode==1)// 1 create
  {
    db_p = create_db(target_name);
    db_ds_p = db_ds_init(db_p,0);
    //init db_ds with semaphore
    Linklist_insert(db_ds_list_p,db_ds_p);
    //printf("thread read- lock_off\n");
    fflush(stdout);
  }
  else if(opcode==2)
  {
    db_p=open_db(target_name);// 2 open
    if(db_p!=NULL)
    {
      db_ds_p = db_ds_init(db_p,0);
      Linklist_insert(db_ds_list_p,db_ds_p);
      //printf("thread_mode- read- lock_off from \n");
      fflush(stdout);
    }
  }
  sem_post(pool_mutex);
  return db_ds_p;
}

db_ds* pool_read_mutiplex_mode(struct Linklist* db_ds_list_p, char* target_name,int opcode)
{
  //printf("mutiplex_mode- read enter\n");
  struct Linklist_node* node_p = Linklist_get_start(db_ds_list_p);
  db_ds* db_ds_p = NULL;
  while(node_p!=NULL)
  {
    db_ds_p = (db_ds*) Linklist_get_data(node_p);
    if(strcmp(target_name,db_ds_p->name)==0)
    {
      if(opcode<=2)
      {
        db_ds_p->flag++;
      }
      //printf("thread_mode- read- exit flag_increase: %d\n",db_ds_p->flag);
      return db_ds_p;
    }
    node_p=Linklist_next_node(node_p);
  }
  struct data_base* db_p;
  db_ds_p=NULL;
  if(opcode==1)// 1 create
  {
    db_p = create_db(target_name);
    db_ds_p = db_ds_init(db_p,1);
    //init db_ds with semaphore
    Linklist_insert(db_ds_list_p,db_ds_p);
    //printf("mutiplex read- create_db\n");
  }
  else if(opcode==2)
  {
    db_p=open_db(target_name);// 2 open
    if(db_p!=NULL)
    {
      db_ds_p = db_ds_init(db_p,1);
      Linklist_insert(db_ds_list_p,db_ds_p);
      //printf("mutiplex_mode-- read- open_db\n");
    }
  }
  // other than 1,2 : nutrual
  return db_ds_p;
}

db_ds* pool_read(struct Linklist* db_ds_list_p, char* target_name, int opcode,sem_t* pool_mutex)
{
  if(pool_mutex==NULL)
    return pool_read_mutiplex_mode(db_ds_list_p, target_name, opcode);
  else
    return pool_read_thread_mode(db_ds_list_p, target_name, opcode, pool_mutex);
}

bool pool_remove_thread_mode(struct Linklist* db_ds_list_p, char* target_name, sem_t* pool_mutex)
{
  sem_wait(pool_mutex);
  //printf("pool remove thread_mode started\n");
  //fflush(stdout);
  struct Linklist_node* node_p = Linklist_get_start(db_ds_list_p);
  db_ds* db_ds_p;
  while(node_p!=NULL)
  {
    db_ds_p = Linklist_get_data(node_p);
    if(strcmp(db_ds_p->db_p->name,target_name)==0)
    {
     // printf("flag %d\n",db_ds_p->flag);
      db_ds_p->flag--;
      if(db_ds_p->flag<1)
      {
        write_data(db_ds_p->db_p);
       // printf("write-\n");
        delete_db_ds(db_ds_p);
        Linklist_remove(db_ds_list_p,node_p);
        //printf("pool remove thread_mode exit\n");
        //fflush(stdout);
      }
        sem_post(pool_mutex);
      return true;
    }
    node_p = Linklist_next_node(node_p);
  }
  //printf("pool remove thread_mode exit\n");
  //fflush(stdout);
  sem_post(pool_mutex);
  return false;
  
}

bool pool_remove_mutiplex_mode(struct Linklist* db_ds_list_p, char* target_name)
{
  //printf("pool remove mutiplex_mode started\n");
  struct Linklist_node* node_p = Linklist_get_start(db_ds_list_p);
  db_ds* db_ds_p;
  while(node_p!=NULL)
  {
    db_ds_p = Linklist_get_data(node_p);
    if(strcmp(db_ds_p->db_p->name,target_name)==0)
    {
      //printf("flag %d\n",db_ds_p->flag);
      db_ds_p->flag--;
      if(db_ds_p->flag<1)
      {
        write_data(db_ds_p->db_p);
        //printf("write-\n");
        delete_db_ds(db_ds_p);
        Linklist_remove(db_ds_list_p,node_p);
        //printf("pool remove mutiplex_mode exit\n");
      }
      return true;
    }
    node_p = Linklist_next_node(node_p);
  }
  //printf("pool remove mutiplex_mode exit\n");
  return false;
}

bool pool_remove(struct Linklist* db_ds_list_p, char* target_name, sem_t* pool_mutex)
{
  if(pool_mutex!=NULL)
    return pool_remove_thread_mode(db_ds_list_p, target_name, pool_mutex);
  else
    return pool_remove_mutiplex_mode(db_ds_list_p, target_name);
}
