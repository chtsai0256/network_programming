#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include "config.h"


extern int cmdCount;

static void doChild(int hasNumPipe, int fd[], char **parsed) {
    if (hasNumPipe) {
        close(fd[WRITE_END]);
        dup2(fd[READ_END], STDIN_FILENO);
        close(fd[READ_END]);
    }
    if (execvp(parsed[0], parsed) < 0) {
        printf("Unknown command: [%s].", parsed[0]);
    }
}

void execArgs(char** parsed)
{

    cmdCount = (cmdCount+1)%MAXCMDNUM;
    bufList_t *buf = getPreResult(cmdCount, 1);

    int fd[2];
    if (buf != NULL) {
        if(pipe(fd)<0) {
            perror("pipe error\n");
            exit(EXIT_FAILURE);
        } 
    }

    switch(fork()) {
        case -1:
            perror("\nFailed forking child..");
            return;
        case 0:
            doChild((buf != NULL), fd, parsed);
            break;
        default:
            close(fd[READ_END]);
            sendNumPipe(buf, fd[WRITE_END]);
            close(fd[WRITE_END]);
            wait(NULL);
    }
}
  