#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include "config.h"

extern command_t *list;
extern buildinCmd_t *bCmdList;
//get number pipe expire buffer
bufList_t * getPreResult(int nowCmdCount, int isNeedReturnList) {
    command_t *ptr = list->next;
    command_t *freePtr = NULL;
    bufList_t *tmp=NULL;
    bufList_t *tail=NULL;

    while(ptr!=NULL && ptr->cmdImportCount<=nowCmdCount) {
        if (isNeedReturnList) {
            if (tmp==NULL) {
                tmp = (bufList_t*)malloc(sizeof(bufList_t));
                memcpy(tmp->data, ptr->buffer, sizeof(ptr->buffer));
                tmp->next = NULL;
                tail = tmp;
            } else {
                bufList_t *temp = (bufList_t *) malloc(sizeof(bufList_t)); 
                memcpy(temp->data, ptr->buffer, sizeof(ptr->buffer));
                temp->next = tmp;
                tmp = temp;
            }
        }
        freePtr = ptr;
        ptr = ptr->next;
        free(freePtr);
    }
    list->next = ptr;
    return tmp;
}

void sendNumPipe(bufList_t *buf, int fd_write_end)
{
    int count = 0;
    while(buf!=NULL) {
        if ((count=write(fd_write_end,buf->data, strlen(buf->data)))<0) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        sleep(0.0005);
        buf = buf->next;
    }
}

static 
char *strstrip(char *s)
{
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}

int
canExec(char *fname) {
    char *path=getenv("PATH");
    char filename[1024];
    char *pathStr;

    while(pathStr = strsep(&path, ":")) {
        sprintf(filename,"%s/%s",strstrip(pathStr), fname);
        if (access(filename, X_OK) == 0)
            return 1;
    }

    if (access(fname, X_OK) == 0)
        return 1;
    else
        return 0;
}

buildinCmd_t * 
findCmd(char *cmd) {
    buildinCmd_t *ptr = bCmdList;
    while (ptr !=NULL) {
        if (strcmp(ptr->command,cmd)==0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

int
createBuildinCmd(char *cmd, void (*func)(void *argv)) {
    if (findCmd(cmd)==NULL) {
        buildinCmd_t *tmp = (buildinCmd_t*)malloc(sizeof(buildinCmd_t));
        strcpy(tmp->command,cmd);
        tmp->func = func;
        if (bCmdList) 
            tmp->next = bCmdList;
        else
            tmp->next = NULL;
        
        bCmdList = tmp;
    }
}
