#include "../include/defs.h"

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

int removeNumPipeEntry(pipeTable *pTable, int removeIndex)
{
    int numReadfd = pTable->table[removeIndex]->numReadfd;
    free(pTable->table[removeIndex]);

    for(int i=removeIndex; i<pTable->curLen - 1; i++)
        pTable->table[i] = pTable->table[i+1];

    pTable->curLen -= 1;
    return numReadfd;
}

int isNumPipeExpire(pipeTable *pTable) // -1: no entry expire, integer: have entry expire.
{
    for(int i=0; i<pTable->curLen; i++)
        if(pTable->table[i]->numPipeCount == 0)
            return removeNumPipeEntry(pTable, i);
    return -1;
}

void parentProc(int *pipefd, int prevReadfd, int numPipefd)
{
    if(pipefd)
        _close(pipefd[1]);
    if(prevReadfd != -1)
        _close(prevReadfd);
    if(numPipefd != -1)
       _close(numPipefd);
    while(waitpid(-1, NULL, 0) > 0)
        continue;
}

void pipeHead(int *pipefd)
{
    _close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    _close(pipefd[1]);
}

void pipeMid(int prevReadfd, int *pipefd)
{
    _close(pipefd[0]);
    dup2(prevReadfd, STDIN_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    _close(pipefd[1]);
    _close(prevReadfd);
}

void pipeTail(int clientfd, int prevReadfd)
{
    dup2(prevReadfd, STDIN_FILENO);
    _close(prevReadfd);
    if(clientfd != -1)
    {
        dup2(clientfd, STDOUT_FILENO);
        _close(clientfd);
    }
}

void numberPipe(int *pipefd, int prevReadfd)
{
    if(prevReadfd != -1)
    {
        dup2(prevReadfd, STDIN_FILENO);
        _close(prevReadfd);
    }
    _close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    _close(pipefd[1]);
}

void procPipe(int clientfd, int *pipefd, int prevReadfd, int pipeOrder, int numPipefd) // have problem
{
    if(numPipefd != -1)
    {
        dup2(numPipefd, STDIN_FILENO);
        _close(numPipefd);
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
        // to clientfd
        pipeTail(clientfd, prevReadfd);
        break;
    case 3:
        numberPipe(pipefd, prevReadfd);
        break;
    default:
        // to clientfd
        if(clientfd != -1)
        {
            dup2(clientfd, STDOUT_FILENO);
            _close(clientfd);
        }
        break;
    }
}

void childProc(command *args, int clientfd, int *pipefd, int prevReadfd, int pipeOrder, int numPipefd)
{
    procPipe(clientfd, pipefd, prevReadfd, pipeOrder, numPipefd);
    if(execvp(args->tokenList[0], args->tokenList) == -1)
        err_sys("execvp error");
    exit(EXIT_FAILURE);
}

void exec(command *args, int clientfd, int *pipefd, int prevReadfd, int pipeOrder, int numPipefd)
{
    flag = 1;
    pid_t pid;
    switch ((pid = fork()))
    {
    case -1:
        err_sys("fork error");
        break;
    case 0:
        childProc(args, clientfd, pipefd, prevReadfd, pipeOrder, numPipefd);
        break;
    default:
        parentProc(pipefd, prevReadfd, numPipefd);
        break;
    }
}

int isNetCmd(char *cmd)
{
    if(!strcmp(cmd, "printenv"))
        return PRINTENV;
    else if(!strcmp(cmd, "setenv"))
        return SETENV;
    else if(!strcmp(cmd, "who"))
        return WHO;
    else if(!strcmp(cmd, "tell"))
        return TELL;
    else if(!strcmp(cmd, "yell"))
        return YELL;
    else if(!strcmp(cmd, "name"))
        return NAME;
    else if(!strcmp(cmd, "quit"))
        exit(EXIT_SUCCESS);
    return -1;
}

void process(command *args, pipeTable *pTable, int clientfd, int *pipefd, int *prevReadfd, int pipeOrder)
{
    int type;
    if((type = isNetCmd(args->tokenList[0])) != -1)
        netService(args, clientfd, type);
    else
    {
        int numPipefd = isNumPipeExpire(pTable);
        exec(args, clientfd, pipefd, *prevReadfd, pipeOrder, numPipefd);
        if(pipefd)
            *prevReadfd = pipefd[0];
    }
}

void procNumberPipe(command *args, pipeTable *pTable, int clientfd, int *prevReadfd)
{
    int numPipeCount = *((args->tokenList[args->curLen-1]) + 1) - '0';
    int pipefd[2];
    if(pipe(pipefd) == -1)
        err_sys("pipe error");
    pTable->table[pTable->curLen++] = getPipeEntry(numPipeCount, pipefd[0]);
    args->tokenList[(--args->curLen)] = NULL;
    process(args, pTable, clientfd, pipefd, prevReadfd, 3);
}

void executeCommand(command *cmd, pipeTable *pTable, int clientfd)
{
    int index = 0, pIndex = 0, lastPipeIndex = 0, pipeOrder = 0;
    // pipeOrder: 0 is head, 1 is mid, 2 is tail, 3 is numberPipe.
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
                err_sys("pipe error");

            args = toSCmd(cmd, index, pIndex);
            process(args, pTable, clientfd, pipefd, &prevReadfd, pipeOrder);
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
        procNumberPipe(args, pTable, clientfd, &prevReadfd);
    else if(pipeOrder)
        process(args, pTable, clientfd, pipefd, &prevReadfd, (++pipeOrder));
    else
        process(args, pTable, clientfd, NULL, &prevReadfd, -1);

    releaseCmd(args);
    releaseCmd(cmd);
}