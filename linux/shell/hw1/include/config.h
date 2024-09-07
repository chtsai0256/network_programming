#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define MAX_BUF_SIZE 10000
#define MAXCMDNUM  32768

#define WRITE_END 1
#define READ_END 0
typedef struct command {
    int cmdImportCount;
    char buffer[MAX_BUF_SIZE];
    struct command *next;
} command_t;

typedef struct buildinCmd {
    char command[MAX_BUF_SIZE];
    void (*func)(void *argv);
    struct buildinCmd *next;
} buildinCmd_t;

typedef struct bufList{
    char data[MAX_BUF_SIZE];
    struct bufList *next;
} bufList_t;


bufList_t * getPreResult(int nowCmdCount, int isNeedReturnList);
buildinCmd_t * findCmd(char *cmd);
