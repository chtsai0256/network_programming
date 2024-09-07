#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include "config.h"

/*
who: show information of all users.
tell: send a message to another user.
yell: send a message to all users.
name: change your name.
*/

static char *
getMessage(char **parsed) {
    char message[1024]={0};
    int i=2;
    while(strlen(parsed[i])>0) 
        sprintf(message, "%s %s", message, parsed[i++]);
    sprintf(message, "%s\n", message);
    return message;
}

void whoHandle(void *arg) {

}
void tellHandle(void *arg) {
    char **parsed = (char **)arg;
    char message = getMessage(parsed);
    printf("send to %s %s\n", parsed[1], message);
}

void yellHandle(void *arg) {
    char **parsed = (char **)arg;
    char message = getMessage(parsed);
    printf("%\n", message);
}

void nameHandle(void *arg) {
    char **parsed = (char **)arg;
    setenv("USER", parsed[1],1);
}

void initBuildinCmdHW2() {
    createBuildinCmd("who", whoHandle);
    createBuildinCmd("tell", tellHandle);
    createBuildinCmd("yell", yellHandle);
    createBuildinCmd("name", nameHandle);
}