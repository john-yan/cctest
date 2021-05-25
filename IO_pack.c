  #include <sys/types.h>
  #include<sys/stat.h>
  #include<fcntl.h>
 #include <stdio.h>
 #include<errno.h>
#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
typedef struct 
{
  int io_fd;
  int io_cnt;
  char* io_bufp;
  char io_buf[8192];
} io_t;

typedef unsigned int length_size_t;
typedef int return_size_t;

ssize_t io_readn(int fd, void* userbuf, size_t n)
{
  size_t number_left =n;
  ssize_t number_read;
  char* bufp = userbuf;

  while(number_left > 0)
  {
    if((number_read = read(fd, bufp , number_left))<0)
    {
      if(errno==EINTR)
        number_read=0;
      else
        return -1;
    }
    else if(number_read ==0)
      break;
    number_left-=number_read;
    bufp += number_read;
  }
  return (n - number_left);

}

ssize_t io_writen(int *rp, void*userbuf, size_t n)
{
  size_t number_left = n;
  ssize_t number_write;
  char* bufp = userbuf;

  while(number_left>0)
  {
    if(number_write=write(rp,bufp,number_left)<=0)
    {
      if(errno==EINTR)
        number_write =0;
      else
        return -1;
    }
    number_left-= number_write;
    bufp+=number_write;
  }
  return n;
}

ssize_t io_readln_buffed(io_t *rp, void* userbuf, size_t maxlen) 
{
  int i;
  int read_char;
  char c,*bufp = userbuf;

  for(i =1;i<maxlen;i++)
  {
    if((read_char = io_read(rp,&c,1)==1))
    {
      *bufp++ = c;
      if(c=='\n')
        i++;
      break;
    }
    else if (read_char==0)
    {
      if(i==1)
        return 0;
      else
        break;
    }
    else
      return -1;
  }
  *bufp =0;
  return i-1;
}

