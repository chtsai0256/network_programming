#include "../include/tools.h"

command* toSCmd(command *cmd, int index, int pIndex)
{
    int size = index - pIndex;
    command *args = (command*)malloc(sizeof(command));
    args->tokenList = (char**)malloc(sizeof(char*) * (size + 1));
    args->curLen = size;
    args->listSize = size + 1;
    
    for(int i=0; i<size; i++)
        args->tokenList[i] = cmd->tokenList[pIndex + i];
    args->tokenList[size] = NULL;
    return args;
}

int isNumberPipe(char *s)
{
    return (s[0] == '|' && *(s+1) != '\0' && isdigit(*(s+1))) ? 1 : 0;
}

int removeNumPipeEntry(int removeIndex)
{
    int numReadfd = pTable.table[removeIndex]->numReadfd;
    free(pTable.table[removeIndex]);
    for(int i=removeIndex; i<pTable.curLen - 1; i++)
        pTable.table[i] = pTable.table[i+1];
    pTable.curLen -= 1;
    return numReadfd;
}

int isNumPipeExpire() // -1: no entry expire, integer: have entry expire.
{
    for(int i=0; i<pTable.curLen; i++)
        if(pTable.table[i]->numPipeCount == 0)
            return removeNumPipeEntry(i);
    return -1;
}

void parentProc(int *pipefd, int prevReadfd, int numPipefd)
{
    if(pipefd)
        close(pipefd[1]);
    if(prevReadfd != -1)
        close(prevReadfd);
    if(numPipefd != -1)
        close(numPipefd);
    while(waitpid(-1, NULL, 0) > 0)
        continue;
}

void pipeHead(int *pipefd)
{
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
}

void pipeMid(int prevReadfd, int *pipefd)
{
    close(pipefd[0]);
    dup2(prevReadfd, STDIN_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    close(prevReadfd);
}

void pipeTail(int prevReadfd)
{
    dup2(prevReadfd, STDIN_FILENO);
    close(prevReadfd);
}

void numberPipe(int *pipefd, int prevReadfd)
{
    if(prevReadfd != -1){
        dup2(prevReadfd, STDIN_FILENO);
        close(prevReadfd);
    }
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
}

void procPipe(int *pipefd, int prevReadfd, int pipeOrder, int numPipefd) // have problem
{
    if(numPipefd != -1)
    {
        dup2(numPipefd, STDIN_FILENO);
        close(numPipefd);
    }

    switch (pipeOrder)
    {
    case 0:
        pipeHead(pipefd);
        break;
    case 1:
        pipeMid(prevReadfd, pipefd);
        break;
    case 2:
        pipeTail(prevReadfd);
        break;
    case 3:
        numberPipe(pipefd, prevReadfd);
        break;
    default:
        break;
    }
}

void childProc(command *args, int *pipefd, int prevReadfd, int pipeOrder, int numPipefd)
{
    procPipe(pipefd, prevReadfd, pipeOrder, numPipefd);
    if(execvp(args->tokenList[0], args->tokenList) == -1)
        printf("Exec error !\n");
    exit(EXIT_FAILURE);
}

void exec(command *args, int *pipefd, int prevReadfd, int pipeOrder, int numPipefd)
{
    pid_t pid;
    switch ((pid = fork()))
    {
    case -1:
        printf("Fork error !\n");
        break;
    case 0:
        childProc(args, pipefd, prevReadfd, pipeOrder, numPipefd);
        break;
    default:
        parentProc(pipefd, prevReadfd, numPipefd);
        break;
    }
}

void process(command *args, int *pipefd, int *prevReadfd, int pipeOrder)
{
    if(!strcmp(args->tokenList[0], "printenv"))
    {

    }else if(!strcmp(args->tokenList[0], "setenv"))
    {

    }else if(!strcmp(args->tokenList[0], "quit"))
    {
        exit(EXIT_SUCCESS);
    }else{
        int numPipefd = isNumPipeExpire();
        exec(args, pipefd, *prevReadfd, pipeOrder, numPipefd);
        if(pipefd)
            *prevReadfd = pipefd[0];
    }
}

void procNumberPipe(command *args, int *prevReadfd)
{
    int numPipeCount = *((args->tokenList[args->curLen-1]) + 1) - '0';
    int pipefd[2];
    pipe(pipefd);
    pTable.table[pTable.curLen++] = getPipeEntry(numPipeCount, pipefd[0]);
    args->tokenList[(--args->curLen)] = NULL;
    process(args, pipefd, prevReadfd, 3);
}

void executeCommand(command *cmd)
{
    int index = 0, pIndex = 0, lastPipeIndex = 0, pipeOrder = 0;
    // pipeOrder: 0 is head, 1 is mid, 2 is tail.
    int *pipefd = NULL;
    command *args;
    int prevReadfd = -1;

    for(; index < cmd->curLen; index ++)
    {
        if( index + 1 < cmd->curLen &&
            strcmp(cmd->tokenList[index], "|") == 0 &&
            isNumberPipe(cmd->tokenList[index]) == 0)
        {   
            pipefd = (int*)malloc(sizeof(int) * 2);
            if(pipe(pipefd) == -1)
            {
                printf("pipe error !\n");
                return ;
            }

            args = toSCmd(cmd, index, pIndex);
            process(args, pipefd, &prevReadfd, pipeOrder);
            releaseCmd(args);
            lastPipeIndex = index;
            pIndex = index + 1;
            pipeOrder = 1;
            free(pipefd);
        }
    }
    pipefd = NULL;
    args = toSCmd(cmd, index, (lastPipeIndex) ? lastPipeIndex + 1 : pIndex);
    
    if(isNumberPipe(args->tokenList[args->curLen-1]))
        procNumberPipe(args, &prevReadfd);
    else if(pipeOrder)
        process(args, pipefd, &prevReadfd, (++pipeOrder));
    else
        process(args, NULL, &prevReadfd, -1);

    releaseCmd(args);
    releaseCmd(cmd);
}