#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main()
{
    char buf[200];
    int fd = open("./test", O_WRONLY,0777);
    if (fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("fd=%d\n",fd); 
    int cnt = read(fd, buf, 100);

    if (cnt == -1) {
        if (errno == EINTR)
            perror("read was interrupted by a signal");
        else {
            //some other error
        }
    }
}
