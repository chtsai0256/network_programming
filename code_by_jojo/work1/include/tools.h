/*----- Include -----*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define PTABLESIZE 512

typedef struct command command;
typedef struct pipeTable pipeTable;
typedef struct pipeEntry pipeEntry;

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

extern pipeTable pTable;

void init();
void shellLoop();

void releaseCmd(command*);
command *parsingCommmand(char*);
int checkCommand(command*);
void executeCommand(command*);
pipeEntry *getPipeEntry(int,int);
void pTalbeClock();