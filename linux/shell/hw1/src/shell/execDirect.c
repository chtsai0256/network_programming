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
        printf("\nCould not execute command..");
    }
}



// Function where the direct commands is executed
void execArgsDirect(char** parsed, char** parsedpipe)
{
    pid_t p1, p2;
    int fd;
 
    cmdCount = (cmdCount+1)%MAXCMDNUM;
    bufList_t *buf = getPreResult(cmdCount, 1);

    int pipefd[2];
    if (buf != NULL) {
        if(pipe(pipefd)<0) {
            perror("pipe error\n");
            exit(EXIT_FAILURE);
        } 
    }

    fd = open(parsedpipe[0],  O_CREAT|O_WRONLY|O_TRUNC, 0666);
    
    switch(fork()) {
        case -1:
            printf("\nCould not fork");
            return;
        case 0:
            dup2(fd, STDOUT_FILENO);
            close(fd);
            doChild((buf!=NULL), pipefd, parsed);
        default:
            close(pipefd[READ_END]);
            sendNumPipe(buf, pipefd[WRITE_END]);
            close(pipefd[WRITE_END]);
            wait(NULL);
            close(fd);
    }
}
