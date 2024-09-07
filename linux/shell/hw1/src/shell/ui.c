#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <limits.h>


#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
  
// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")
  
void init_env(char *username)
{
    char path[1024];
    getcwd(path, sizeof(path));
    sprintf(path,"%s/bin:.",path);
    clearenv();
    setenv("USER", username,1);
    setenv("PATH",path,1);
}

// Greeting shell during startup
void init_shell()
{
    clear();
    char* username = getenv("USER");

    init_env(username);
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    initBuildinCmd();
    sleep(0.5);
    clear();
}
  
// Function to take input
int takeInput(char* str)
{
    char* buf;
  
    buf = readline("\n% ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
  
// Function to print Current Directory.
void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    //printf("\n%s", cwd);
}
  
