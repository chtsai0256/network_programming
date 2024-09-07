#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include "config.h"

int cmdCount;
command_t *list;
buildinCmd_t *bCmdList=NULL;

int startShell()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    int numCmd=0;
    init_shell();
  
    cmdCount = 0 % MAXCMDNUM;
    list = (command_t *) malloc(sizeof(command_t));
    list->next = NULL;
    while (1) {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
        parsedArgs, parsedArgsPiped, &numCmd);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.
        // 3 if it is including a direct
        // 4 if it is including a num pipe

        // execute
        if (execFlag == 1) {
            if (canExec(parsedArgs[0]))
                execArgs(parsedArgs);
            else {
                printf("Unknown command: [%s].\n",parsedArgs[0]);
            }
        }
  
        if (execFlag == 2) {
            if (canExec(parsedArgs[0]) && canExec(parsedArgsPiped[0]))
                execArgsPiped(parsedArgs, parsedArgsPiped);
            else if (canExec(parsedArgsPiped[0])) {
                printf("Unknown command: [%s].\n",parsedArgs[0]);
                execArgs(parsedArgsPiped);
            } else {
                printf("Unknown command: [%s].\n",parsedArgsPiped[0]);
            }
        }
        if (execFlag == 3)
            if (canExec(parsedArgs[0]))
                execArgsDirect(parsedArgs, parsedArgsPiped);
            else {
                printf("Unknown command: [%s].\n",parsedArgs[0]);
            }

        if (execFlag == 4)
            if (canExec(parsedArgs[0]))
                execArgsNumPiped(parsedArgs, numCmd);
            else {
                printf("Unknown command: [%s].\n",parsedArgs[0]);
            }
    }
    return 0;
}
