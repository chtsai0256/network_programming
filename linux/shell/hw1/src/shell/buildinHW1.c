#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include "config.h"

extern char **environ;

// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    char* username;
    buildinCmd_t *  bCmd = findCmd(parsed[0]);

    if (bCmd) {
        bCmd->func(parsed);
        return 1;
    } else {
        return 0;
    }
}

void quit(void *arg){
    printf("\nGoodbye\n");
    exit(EXIT_SUCCESS);
}

void changeDir(void *arg) {
    char **parsed = (char **)arg;
    chdir(parsed[1]);
}

// Help command builtin
void openHelp(void *arg)
{
    puts("\n***WELCOME TO SHELL HELP***"
        "\n-Use the shell at your own risk..."
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>setenv"
        "\n>printenv"
        "\n>quit or exit"
        "\n>all other general commands available in bin"
        "\n>normal pipe handling"
        "\n>number pipe handling");
}
 
void mySetenv(void *arg) {
    char **parsed = (char **)arg;
    setenv(parsed[1], parsed[2], 1);
} 

void printenv(void *arg){
    char **parsed = (char **)arg;
    char *name = parsed[1];
    char **ep;

    if (name==NULL) {
        for (ep = environ; *ep != NULL; ep++)
            printf("%s\n", *ep);
    }else{
        char *value = getenv(name);
        if (value==NULL)
            printf("\n");
        else
            printf("%s\n", value);
    }
}


void initBuildinCmd() {
    createBuildinCmd("quit", quit);
    createBuildinCmd("exit", quit);
    createBuildinCmd("cd", changeDir);
    createBuildinCmd("help", openHelp);
    createBuildinCmd("setenv", mySetenv);
    createBuildinCmd("printenv", printenv);
}