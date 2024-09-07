#include "../include/defs.h"

char clientFifo[CLIENT_FIFI_LEM];

static void removeFifo(void)
{
    unlink(clientFifo);
}

void initPath()
{
	setenv("PATH", "bin:.", 1);	
}

void eliReturnChar(char *cmd)
{
    int cmdLen = strlen(cmd);
    cmd[cmdLen-1] = '\0';
    cmd[cmdLen-2] = '\0';
}

void initShell(pipeTable *pTable)
{
    initPath();
    pTable->tableSize = PTABLESIZE;
    pTable->curLen = 0;
    pTable->table = (pipeEntry**)malloc(sizeof(pipeEntry*) * pTable->tableSize);

    snprintf(clientFifo, CLIENT_FIFI_LEM, CLIENT_FIFO_TMP, (long)getpid());

    if(mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        err_sys("mkfifo error");

    if(atexit(removeFifo) != 0)
        err_sys("atexit error");
}

int flag;

void startShell(int clientfd)
{
    pipeTable pTable;
    pthread_t tid;

    initShell(&pTable);
    pthread_create(&tid, NULL, receiveResp, (void*)&clientfd);
    flag = 1;

    while(1)
    {
        char buf[CMDSIZE] = {'\0'};
        size_t bufLen = 0;
        int cmdLen;
        
        if(flag)
            send(clientfd, "% ", strlen("% "), 0);
        flag = 0;
        recv(clientfd, buf, CMDSIZE, 0);
        eliReturnChar(buf);

        command *cmd = parsingCommmand(buf);
        if(cmd)
            executeCommand(cmd, &pTable, clientfd);
 
        pTalbeClock(&pTable);
    }
}