#include "../include/defs.h"

int isUserOnline(int id)
{
    for(int i=0; i<USERTABLESIZE; i++)
        if( cuTable.table[i] &&
            cuTable.table[i]->id == id)
            return 1;
    return 0;
}

void sendToClient(int pid, int mLen, char *message)
{
    char clientFifo[CLIENT_FIFI_LEM] = {'\0'};
    snprintf(clientFifo, CLIENT_FIFI_LEM, CLIENT_FIFO_TMP, (long)pid);

    int cfd = open(clientFifo, O_WRONLY);

    _write(cfd, &mLen, 4);
    _write(cfd, message, mLen);
    _close(cfd);
}

void who(int pid)
{
    char buf[MAXLEN] = {'\0'};
    sprintf(buf, "%s", "<ID>    <name>    <IP:port>          <indicate me>\n");
    for(int i=0; i<USERTABLESIZE; i++)
        if(cuTable.table[i])
            sprintf((buf + strlen(buf)), "%4d    %-10s%-15s    %s\n",
                            cuTable.table[i]->id,
                            cuTable.table[i]->name,
                            cuTable.table[i]->ipp,
                            (cuTable.table[i]->pid == pid) ? "<-(me)" : "");
    
    sendToClient(pid, strlen(buf), buf);
    sendToClient(pid, 0, NULL);
}

void tell(int pid, char *args)
{
    char message[MLEN] = {'\0'};
    int userid = 0;
    int i, base, index;

    for(i=0, base=1; args[i] != ' '; i++, base*=10)
    {
        userid *= base;
        userid = ((args[i] - '0') + userid);
    }

    for(index=0; index<USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;

    if(isUserOnline(userid))
    {
        for(i=0; i<USERTABLESIZE; i++)
            if(cuTable.table[i] && cuTable.table[i]->id == userid)
                break;

        sprintf(message, "<user(%d) told you>: ", cuTable.table[index]->id);
        sprintf(message + strlen(message), "%s\n", &args[i+1]);
        sendToClient(cuTable.table[i]->pid, strlen(message), message);
        sendToClient(cuTable.table[i]->pid, 0, NULL);
        sendToClient(pid, strlen("send accept!\n"), "send accept!\n");
    }else
    {
        sprintf(message, "%s", "Error: user does not exist yet.\n");
        sendToClient(pid, strlen(message), message);
    }
    sendToClient(pid, 0, NULL);
}

void yell(int pid, char *args)
{
    char message[MLEN] = {'\0'};
    int index;

    for(index=0; index<USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;

    sprintf(message, "<user(%d) yelled>: ", cuTable.table[index]->id);
    sprintf(message + strlen(message), "%s\n", args);
    
    for(int i=0; i<USERTABLESIZE; i++)
    {
        if(cuTable.table[i])
        {   
            sendToClient(cuTable.table[i]->pid, strlen(message), message);
            sendToClient(cuTable.table[i]->pid, 0, NULL);
        }
    }
}

void name(int pid, char *args)
{
    int index;

    for(index=0; index<USERTABLESIZE; index++)
        if(cuTable.table[index] && cuTable.table[index]->pid == pid)
            break;
    
    memset(cuTable.table[index]->name, '\0', USERNAMESIZE);
    strcpy(cuTable.table[index]->name, args);

    sendToClient(pid, strlen("name change accept!\n"), "name change accept!\n");
    sendToClient(pid, 0, NULL);
}

void requestHandler(int recvfd)
{
    int aLen, cmd, pid;
    char args[40] = {'\0'};
    
    _read(recvfd, &pid, 4);
    _read(recvfd, &cmd, 4);
    _read(recvfd, &aLen, 4);
    _read(recvfd, args, aLen);

    switch (cmd)
    {
    case PRINTENV:
        break;
    case SETENV:
        break;
    case WHO:
        who(pid);
        break;
    case TELL:
        tell(pid, args);
        break;
    case YELL:
        yell(pid, args);
        break;
    case NAME:
        name(pid, args);
        break;
    default:    // error
        break;
    }
}