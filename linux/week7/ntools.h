#include "myhdr.h"
#define MAXLINE 1024
#define SERV_PORT 9877

ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, const void *buff, size_t n);
ssize_t readline(int fd, void *buff, size_t maxlen);


typedef struct args {
    long arg1;
    long arg2;
} args_t;

typedef struct result {
    long sum;
} result_t;