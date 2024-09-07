#include "../include/defs.h"

void sendRequest(command *args, int type)
{
    int servfd = open(SERVERR_FIFO, O_WRONLY);
    int option = PHANDLER;
    int pid = getpid();
    int offset = 0;
    int bufLen = 0;
    char buf[MLEN] = {'\0'};

    _write(servfd, &option, 4);
    _write(servfd, &pid, 4);
    _write(servfd, &type, 4);

    for(int i=1; i<args->curLen; i++)
    {
        sprintf(buf + offset, "%s\0", args->tokenList[i]);
        bufLen += (strlen(args->tokenList[i]) + 1);
        offset += (strlen(args->tokenList[i]) + 1);
    }

    _write(servfd, &bufLen, 4);
    _write(servfd, buf, bufLen);
    _close(servfd);
}

void* receiveResp(void *args)
{
    int clientfd = *(((argToT*)args)->cfd);
    char buffer[MAXLEN] = {'\0'};
    char clientFifo[CLIENT_FIFI_LEM] = {'\0'};
    snprintf(clientFifo, CLIENT_FIFI_LEM, CLIENT_FIFO_TMP, (long)getpid());

    int fd = open(clientFifo, O_RDONLY);
    int mLen;
    
    while(1)
    {
        _read(fd, &mLen, 4);
        if(mLen != 0)
        {
            _read(fd, buffer, mLen);
            send(clientfd, buffer, mLen, 0);
            send(clientfd, ((argToT*)args)->prompt, strlen(((argToT*)args)->prompt), 0);
        }
    }
    _close(fd);
    pthread_exit(NULL);
}

void netService(command *args, int type)
{
    //isCmdLegal() check cmd is legal.
    sendRequest(args, type);
}