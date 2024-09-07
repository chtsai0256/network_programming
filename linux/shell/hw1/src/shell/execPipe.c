#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include "config.h"

extern int cmdCount;

static void doChild(int hasNumPipe, int numPipefd[], int pipefd[], char **parsed) 
{
        if (hasNumPipe) {
            close(numPipefd[WRITE_END]);
            dup2(numPipefd[READ_END], STDIN_FILENO);
            close(numPipefd[READ_END]);
        }

        close(pipefd[READ_END]);
        dup2(pipefd[WRITE_END], STDOUT_FILENO);
        close(pipefd[WRITE_END]);
    
        if (execvp(parsed[0], parsed) < 0) {
            printf("Unknown command: [%s].\n", parsed[0]);
            exit(EXIT_FAILURE);
        }
}

static void doParent(int pipefd[], char **parsedpipe)
{
    switch(fork()) {
        case -1:
            printf("\nCould not fork");
            return;
        case 0:
            close(pipefd[WRITE_END]);
            dup2(pipefd[READ_END], STDIN_FILENO);
            close(pipefd[READ_END]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("Unknown command: [%s].\n", parsedpipe[0]);
                return;
            }
        default:
            close(pipefd[WRITE_END]);
            close(pipefd[READ_END]);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe)
{
    cmdCount = (cmdCount+1)%MAXCMDNUM;
    bufList_t *buf = getPreResult(cmdCount,1);

    int numPipefd[2];
    
    if (buf != NULL) {
        if(pipe(numPipefd)<0) {
            perror("pipe error\n");
            exit(EXIT_FAILURE);
        } 
    }

    int pipefd[2]; 
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }

    switch(fork()) {
        case -1:
            printf("\nCould not fork");
            return;
        case 0:
            doChild((buf!=NULL), numPipefd, pipefd, parsed);
            break;
        default:            
            doParent(pipefd, parsedpipe);
            close(numPipefd[READ_END]);
            sendNumPipe(buf, numPipefd[WRITE_END]);
            close(numPipefd[WRITE_END]);
            wait(NULL);
            wait(NULL);
    }
}








// Function where the piped system commands is executed
void execArgsPiped1(char** parsed, char** parsedpipe)
{
    cmdCount = (cmdCount+1)%MAXCMDNUM;
    bufList_t *buf = getPreResult(cmdCount,1);

    int fd[2];
    
    if (buf != NULL) {
        if(pipe(fd)<0) {
            perror("pipe error\n");
            exit(EXIT_FAILURE);
        } 
    }

    // 0 is read end, 1 is write end
    int pipefd[2]; 
    int pipefd1[2];     
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    if (pipe(pipefd1) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }

    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }


    if (p1 == 0) {

        if (buf != NULL) {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
        }

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        close(pipefd1[1]);
        close(pipefd1[0]);
    
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing

        p2 = fork();
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        } 
        
        if (p2==0){
            // Child 2 executing..
            // It only needs to read at the read end
            dup2(pipefd1[0], STDIN_FILENO);
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd[1]);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        }
        
        int count = 0;
        if (buf !=NULL) {
            while(buf!=NULL) {
                if ((count=write(fd[1],buf->data, strlen(buf->data)+1))<0) {
                    perror("write\n");
                    exit(EXIT_FAILURE);
                }
                buf = buf->next;
            }
        }
        
        int len;
        char buffer[1024]={0};
        close(pipefd[1]);
        close(pipefd1[0]);
        while((len=read(pipefd[0], buffer, 1023))>0) {
            buffer[len] = '\0';
            len = write(pipefd1[1], buffer, strlen(buffer)+1);
        }
        close(pipefd[0]);
        close(pipefd1[1]);
        wait(NULL); 
        wait(NULL);

    }
}
  

