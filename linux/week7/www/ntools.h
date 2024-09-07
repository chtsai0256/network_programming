#include "myhdr.h"
#define MAXLINE 1024
#define SERV_PORT 8977
#define SERV_IP "134.208.6.49"

ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, const void *buff, size_t n);
ssize_t readline(int fd, void *buff, size_t maxlen);

