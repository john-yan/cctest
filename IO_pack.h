#include<sts/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<errno.h>

typedef struct 
{
  int io_fd;
  int io_cnt;
  char* io_bufp;
  char io_buf[8192];
} io_t;

ssize_t io_readn(int fd, void* userbuf, size_t n);

ssize_t io_writen(int *rp, void* userbuf, size_t n);

ssize_t io_wrten_buff(io_t* rp , void* userbuf, size_t n);

ssize_t io_readline_buff(io_t *rp, void* userbuf, size_t maxlen);
