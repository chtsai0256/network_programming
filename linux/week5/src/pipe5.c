#include <sys/wait.h>
#include "myhdr.h"
#include <string.h>

void 
doChild(int fd[], char *commandArgv[]) {
    //output to parent
    close(fd[READ_END]);
    dup2(fd[WRITE_END], STDOUT_FILENO);
    close(fd[READ_END]);
    if (execvp(commandArgv[0], commandArgv) < 0) {
        perror("execvp");
        exit(EXIT_FAILURE);
    };
}

void 
doParent(int fd[]) {
    close(fd[WRITE_END]);
    dup2(fd[READ_END], STDIN_FILENO);
    close(fd[READ_END]);

    char buf[1024];
    int cnt=1;
    while(scanf("%s", buf)!=EOF)
        printf("%d %s\n", cnt++, buf);
}

int
main(int argc, char *argv[])
{
    int fd[2];

    char *exeArgv0[50];

    exeArgv0[0] = "ls";
    exeArgv0[1] = NULL;

    if ( (pipe(fd) == -1) )  {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            doChild(fd, exeArgv0);
            break;
        default:
            doParent(fd);
            wait(NULL);
            break;
    }
    exit(EXIT_SUCCESS);
}               