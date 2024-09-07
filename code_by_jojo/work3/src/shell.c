#include "../include/defs.h"

char clientFifo[CLIENT_FIFI_LEM];
int flag;                           // for "%" print problem
int isCmdPass;                      // for number pipe ignore unknown  

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
    flag = 1;
    isCmdPass = 1;
    pTable->tableSize = PTABLESIZE;
    pTable->curLen = 0;
    pTable->table = (pipeEntry**)malloc(sizeof(pipeEntry*) * pTable->tableSize);

    for(int i=0; i<PTABLESIZE; i++)
        pTable->table[i] = NULL;

    snprintf(clientFifo, CLIENT_FIFI_LEM, CLIENT_FIFO_TMP, (long)getpid());

    if(mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        err_sys("mkfifo error");
}

void startShell(int clientfd, char *user)
{
    pipeTable pTable;
    pthread_t tid;
    argToT toThread;
    char prompt[STR100] = {'\0'};

    initShell(&pTable);
    sprintf(prompt, "(%s)%s", user, "% ");
    toThread.cfd = &clientfd;
    toThread.prompt = &prompt;

    pthread_create(&tid, NULL, receiveResp, (void*)&toThread);
    free(user);

    while(1)
    {
        char buf[CMDSIZE] = {'\0'};
        size_t bufLen = 0;
        int cmdLen;

        if(flag)
            send(clientfd, prompt, strlen(prompt), 0);
        
        recv(clientfd, buf, CMDSIZE, 0);
        eliReturnChar(buf);

        if(strlen(buf))
            flag = 0;
        else
            isCmdPass = 0;
        
        command *cmd = parsingCommmand(buf);

        if(cmd)
            executeCommand(cmd, &pTable, clientfd);
        if(isCmdPass)
        {
            removeNumPipeEntry(&pTable);
            pTalbeClock(&pTable);
        }

        isCmdPass = 1;  // reset to true
    }
}