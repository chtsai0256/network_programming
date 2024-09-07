#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include "config.h"

extern command_t *list;
extern int cmdCount;

static void
doParent(int fd_read_end, int numCmd)
{
    command_t *node = (command_t *)malloc(sizeof(command_t));
    memset(node->buffer,0,sizeof(node->buffer));
    cmdCount = (cmdCount+1) % MAXCMDNUM;
    node->cmdImportCount = numCmd + cmdCount;
    
    int count = read(fd_read_end, node->buffer, sizeof(node->buffer));
    node->buffer[count] = '\0';
    close(fd_read_end);

    command_t *ptr=list;
    while(ptr->next !=NULL && ptr->next->cmdImportCount <= node->cmdImportCount)
        ptr = ptr->next;
    node->next = ptr->next;
    ptr->next = node;
    
    getPreResult(cmdCount,0);
}

// Function where the number piped system commands is executed
void execArgsNumPiped(char** parsed, int numCmd)
{
    int fd[2];

    if (pipe(fd)<0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
        case -1:
            printf("\nCould not fork");
            return;
        case 0:
            close(fd[READ_END]);
            dup2(fd[WRITE_END], STDOUT_FILENO);
            close(fd[WRITE_END]);
    
            if (execvp(parsed[0], parsed) < 0) {
                printf("\nCould not execute command 1..");
                exit(0);
            }
            break;
        default:
            close(fd[WRITE_END]);
            wait(NULL);
            doParent(fd[READ_END], numCmd);
    }
}

