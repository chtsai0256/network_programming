#include "../include/defs.h"

/*---------- for cUserTable operation ----------*/

int getUserId()
{
    int i=0;
    for(; i<USERTABLESIZE; i++)
        if(cuTable.table[i] == NULL)
            break;
    return i;
}

user *getUser(char *ipp, int cfd, int pid, int id)
{
    user *u = (user*)malloc(sizeof(user));
    memset(u->name, '\0', 20);
    strcpy(u->name, "no_name");
    u->ipp = (char*)malloc(sizeof(char) * strlen(ipp));
    strcpy(u->ipp, ipp);
    u->password = NULL;
    u->pid = pid;
    u->id = id;
    return u;
}

void releaseUser(user *u)
{
    free(u->ipp);
    free(u->password);
    free(u);
}

void addToCUTable(char *ipp, int cfd, int pid)
{
    int id = getUserId();
    user *u = getUser(ipp, cfd, pid, id);
    cuTable.table[id] = u;
}

void removeUserFromCUTable(int cpid)
{
    int i;
    for(i=0; i<USERTABLESIZE; i++)
        if( cuTable.table[i] &&
            cuTable.table[i]->pid == cpid)
            break;

    releaseUser(cuTable.table[i]);
    cuTable.table[i] = NULL;
}