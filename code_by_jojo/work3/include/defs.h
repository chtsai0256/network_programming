/*-------------------- <fifo>  --------------------*/ 
#include <sys/stat.h>

#define USERLOGIN 0
#define USEREXIT 1

#define UHANDLER 0
#define PHANDLER 1

#define SERVERR_FIFO "/tmp/sRead"
#define SERVER_FIFO_RECV "/tmp/sRecv"

#define CLIENT_FIFO_TMP "/tmp/clientRecv_%ld"
#define CLIENT_FIFI_LEM (sizeof(CLIENT_FIFO_TMP) + 20)

/*-------------------- <shell>  --------------------*/ 
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PTABLESIZE 128 /* numberPipe table size */
#define CMDSIZE 512

typedef struct command command;
typedef struct pipeTable pipeTable;
typedef struct pipeEntry pipeEntry;
typedef struct argToT argToT;

extern char clientFifo[CLIENT_FIFI_LEM];
extern int flag;
extern int isCmdPass;

struct command
{
    size_t listSize;
    size_t curLen;
    char **tokenList;
};

struct pipeEntry
{
    size_t numPipeCount;
    int numReadfd;
};

struct pipeTable
{
    size_t tableSize;
    size_t curLen;
    pipeEntry **table;
};

/*---------- Shell ----------*/

// args send to thread
struct argToT
{
    int *cfd;
    char *prompt;
};

void startShell(int, char*);
void childPrintenv();
void childSetenv();

void* receiveResp(void*);
void eliReturnChar(char*);

/*---------- NumberPipe ----------*/
pipeEntry *getPipeEntry(int,int);
void pTalbeClock(pipeTable*);

void removeNumPipeEntry(pipeTable*);

/*---------- Command ----------*/
void releaseCmd(command*);
int checkCommand(command*);
void executeCommand(command*, pipeTable*, int);
command *parsingCommmand(char*);

/*-------------------- <server>  --------------------*/ 
#include <arpa/inet.h>	/* inet(3) functions */
#include <fcntl.h>		/* for nonblocking */
#include <netdb.h>      /* for getaddrinfo */  
#include <netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>		/* for Unix domain sockets */

#define PRINTENV 0
#define SETENV 1
#define WHO 2
#define TELL 3
#define YELL 4
#define NAME 5

#define	SA struct sockaddr

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ 1024    /* 2nd argument to listen() */
#define	MAXLEN	4096    /* max text line length */
#define MLEN 1024
#define STR100 100

#define USERTABLESIZE 35

#define USERNAMESIZE 20

typedef struct user user;
typedef struct cUserTable cUserTable;
typedef struct control control;

extern cUserTable cuTable;

struct control
{
    int upOption;
    int isLogin;
    int len;
    char *ipp;
    int pid;
};

struct user
{
    char name[USERNAMESIZE];
    //char *password;             // user password
    char *ipp;                  // store ip & port
    int id;                     // client id
    int pid;                    // client pid
};

struct cUserTable
{
    user *table[USERTABLESIZE];
};

char *Sock_ntop(const struct sockaddr*, socklen_t);
int tcpListen(const char*, const char*, socklen_t*);

/*---------- user table operation ----------*/
void addToCUTable(char*, int, int, char*);
void removeUserFromCUTable(int);

/*---------- wsocket ----------*/
int _accept(int, struct sockaddr*, socklen_t*);

/*---------- wunix ----------*/
void _close(int);
ssize_t _read(int, void*, size_t);
void _write(int, void*, size_t);

/*---------- network service ----------*/
void netService(command*, int);
void requestHandler(int);
void childWho();
void childTell();
void childYell();
void childName();

/*---------- error ----------*/
// extern int desfd;
void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

/*---------- Mail box ----------*/ 
#define MAILTO 6
#define DELMAIL 7
#define LISTMAIL 8

/*---------- Group ----------*/ 
#define CREATEGROUP 9
#define DELGROUP 10
#define ADDTO 11
#define LEAVEGROUP 12
#define LISTGROUP 13
#define GYELL 14
#define REMOVE 15

/*---------- login ----------*/

#define USEREXIST 0
#define USERUNEXIST 1
#define PASSWORDERR 2

char* login(int, int, control*);

/*-------------------- <database> --------------------*/ 
#include <hiredis/hiredis.h>

redisContext* dbConnect();
char* dbList(redisContext*, char*);
void dbDelete(redisContext*, char*);
void dbSet(redisContext*, char*, char*);
void dbPushToList(redisContext*, char*, char*);

int checkUser(redisContext*, char*, char**, int);