#include <sys/wait.h>
#include "myhdr.h"
#include <string.h>

int
main(int argc, char *argv[])
{
    char buf[1024];
    char *exeArgv0[50];
    char *exeArgv1[50];

    exeArgv0[0] = "ls";
    exeArgv0[1] = NULL;

    exeArgv1[0] = "./sbin/number";//argv[2];
    exeArgv1[1] = NULL;
    
    subprocess_t wChild = subprocess_out(exeArgv0);
    //subprocess_t rChild = subprocess_in(exeArgv1);
    
    sleep(3);
    int numReads = read(wChild.fd, &buf, 1024);
    if (numReads<0) {
        perror("read fd");
        exit(EXIT_FAILURE);
    }

    while (numReads > 0) {
        printf("ls:%s",buf);
        //write(rChild.fd, &buf, 1024);
        numReads = read(wChild.fd, &buf, 1024);
    }
    //buf = EOF;
    //write(fd1[WRITE_END], &buf, 1);
    close(wChild.fd);
    wait(NULL);
    wait(NULL);
    exit(EXIT_SUCCESS);
}               