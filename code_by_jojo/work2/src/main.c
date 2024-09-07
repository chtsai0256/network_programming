#include "../include/defs.h"

int serrfd;

void printTable()
{
    printf("\n");
    for(int i=0; i<USERTABLESIZE; i++)
        if(cuTable.table[i])
            printf("process id: %d, ip & port: %s\n",
                                                  cuTable.table[i]->pid,
                                                  cuTable.table[i]->ipp);
    printf("\n");
}

/*----------- SIGCHILD signal handler ----------*/
static void sigChildHandler(int sig)
{
    int savedErrno, cpid;
    int option = USEREXIT;
    int control = UHANDLER;
    savedErrno = errno;

    while((cpid = waitpid(-1, NULL, WNOHANG)) > 0 )
    {
        _write(serrfd, &control, 4);
        _write(serrfd, &option, 4);
        _write(serrfd, &cpid, 4);
        continue;
    }
    errno = savedErrno;
}

static void prepareSig()
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigChildHandler;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
        err_sys("Error for signal setting !\n");
}

/*----------- server for listen  ----------*/

void childServ(int clientfd)
{
    startShell(clientfd);
    _close(clientfd);
}

void createServ(int clientfd)
{
    pid_t pid;
    switch ((pid = fork()))
    {
    case -1:
        err_sys("fork error");
        break;
    case 0:
        int tmp = getpid();
        _write(serrfd, &tmp, 4);
        _close(serrfd);
        childServ(clientfd);
        break;
    default:
        _close(clientfd); 
        break;
    }
}

void startServ(char *port)
{
    int				listenfd, clientfd, oLen, control;
	socklen_t		len;
	struct sockaddr_storage	cliaddr;
    char *ipp;

    serrfd = open(SERVERR_FIFO, O_WRONLY);
    oLen = USERLOGIN;
    control = UHANDLER;

    prepareSig(serrfd);
	listenfd = tcpListen(NULL, port, NULL);

	for( ; ; )
    {
		len = sizeof(cliaddr);
		clientfd = _accept(listenfd, (SA *)&cliaddr, &len);
 
        ipp = Sock_ntop((SA *)&cliaddr, len);
        len = strlen(ipp);
        _write(serrfd, &control, 4);            // descide to userHandler or processhandler
        _write(serrfd, &oLen, 4);               // descide login or login out
        _write(serrfd, &len, 4);                // ip & port length
        _write(serrfd, ipp, strlen(ipp));       // ip & port

        createServ(clientfd);
	}
    close(serrfd);
}

/*----------- server for communicate with client  ----------*/
cUserTable cuTable;

void initServ()
{
    cuTable.index = 0;
    for(int i=0; i<USERTABLESIZE; i++)
        cuTable.table[i] = NULL;
}

void userLogin(int fd)
{
    int mLen, cfd, pid;
    char ipInfo[20] = {'\0'};
    _read(fd, &mLen, 4);
    _read(fd, ipInfo, mLen);
    _read(fd, &pid, 4);

    addToCUTable(ipInfo, cfd, pid);
}

void userExit(int fd)
{
    int cpid;
    _read(fd, &cpid, 4);
    printf("\nuser exit %d\n", cpid);
    removeUserFromCUTable(cpid);
}

void userHandler(int fd)
{
    int option;
    _read(fd, &option, 4);

    if(option == USERLOGIN)
        userLogin(fd);
    else if(option == USEREXIT)
        userExit(fd);

    printTable();
}

void serverForProcess()
{
    initServ();

    if(mkfifo(SERVERR_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        err_sys("mkfifo error");
    int fd = open(SERVERR_FIFO, O_RDONLY);

    while(1)
    {
        int option = -1;
        _read(fd, &option, 4);
        if(option == UHANDLER)
            userHandler(fd);
        else if(option == PHANDLER)
            requestHandler(fd);
    }
    _close(fd);
}

int main(int argc, char **argv)
{
    if(argc != 2)
        err_quit("usage: jserver <service or port#>");
    
    pid_t pid;
    if((pid = fork()) == 0)
        startServ(argv[1]);
    else
        serverForProcess();

    exit(EXIT_SUCCESS);
}