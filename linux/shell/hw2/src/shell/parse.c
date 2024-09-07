#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include "config.h"


// function for finding normal pipe=1, number pipe=2
static int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
  
    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        if (!isdigit(strpiped[1][0]))
            return 1;
        else
            return 2;
    }
}
  
// function for finding direct
static int parseDirect(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, ">");
        if (strpiped[i] == NULL)
            break;
    }
  
    if (strpiped[1] == NULL)
        return 0; // returns zero if no direct is found.
    else {
        return 1;
    }
}

// function for parsing command words
static void parseSpace(char* str, char** parsed)
{
    int i;
  
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
  
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}
  
int processString(char* str, char** parsed, char** parsedpipe, int *numCmd)
{
  
    char* strpiped[2];
    int piped = 0;
    int direct = 0;
    int instType = 1;

    piped = parsePipe(str, strpiped);

    if (piped==2)
        *numCmd = atoi(strpiped[1]); 
    else
        *numCmd = 0;

    if (piped==1) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
        instType = 2;
    } else if (piped==2) {
        parseSpace(strpiped[0], parsed);
        instType = 4;
    } else {
        direct = parseDirect(str, strpiped);
        if (direct==1) {
            parseSpace(strpiped[0], parsed);
            parseSpace(strpiped[1], parsedpipe);
            instType = 3;
        } else 
            parseSpace(str, parsed);
    }
  
    if (ownCmdHandler(parsed))
        return 0;
    else
        return instType;
}

