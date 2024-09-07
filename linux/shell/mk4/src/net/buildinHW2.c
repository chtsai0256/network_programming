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

void whoHandle(void *arg) {
    char **parsed = (char **)arg;
    
}

void tellHandle(void *arg) {
    char **parsed = (char **)arg;
    //Format: tell user id message
    
}

void yellHandle(void *arg) {
    char **parsed = (char **)arg;
    
}
void nameHandle(void *arg) {
    char **parsed = (char **)arg;
    // Format : name ‘new name’
    
}

void initBuildinCmd_hw2() {
    createBuildinCmd("who", whoHandle);
    createBuildinCmd("tell", tellHandle);
    createBuildinCmd("yell", yellHandle);
    createBuildinCmd("name", nameHandle);
}