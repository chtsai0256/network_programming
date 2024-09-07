#include "../include/tools.h"

void initPath()
{
	setenv("PATH", "bin:.", 1);	
}

void init()
{
    printf("\033[H\033[J");
    pTable.tableSize = PTABLESIZE;
    pTable.curLen = 0;
    pTable.table = (pipeEntry**)malloc(sizeof(pipeEntry*) * pTable.tableSize);
    initPath();
}

void shellLoop()
{
    while(1)
    {
        char *buf = NULL;
        size_t bufLen = 0;
        int cmdLen;

        printf("$ ");
        getline(&buf, &bufLen, stdin);

        command *cmd = parsingCommmand(buf);
        if(cmd && checkCommand(cmd))
            executeCommand(cmd);
 
        free(buf);
        pTalbeClock();
    }
}