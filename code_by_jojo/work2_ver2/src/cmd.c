#include "../include/defs.h"

void releaseCmd(command *cmd)
{
    free(cmd->tokenList);
    free(cmd);
}

pipeEntry *getPipeEntry(int number, int numPipeFd)
{
    pipeEntry *entry = (pipeEntry*)malloc(sizeof(pipeEntry));
    entry->numPipeCount = number;
    entry->numReadfd = numPipeFd;
    return entry;
}

void pTalbeClock(pipeTable *pTable)
{
    for(int i=0; i<pTable->curLen; i++)
        if(pTable->table[i]->numPipeCount > 0)
            pTable->table[i]->numPipeCount -= 1;
}

command *parsingCommmand(char *input)
{
    int index = 0;
    char *delim = " \f\r\t\v\n";
    char *token = strtok(input, delim);

    if(!token)
        return NULL;

    command *cmd = (command*)malloc(sizeof(command));
    cmd->listSize = 1;
    cmd->tokenList = (char**)malloc(sizeof(char*));

    while(token)
    {
        if(index >= cmd->listSize)
        {
            cmd->listSize *= 2;
            cmd->tokenList = (char**)realloc(cmd->tokenList, sizeof(char*) * cmd->listSize);
        }
        cmd->tokenList[index] = token;
        index ++;
        token = strtok(NULL, delim);
    }
    cmd->curLen = index;
    return cmd;
}