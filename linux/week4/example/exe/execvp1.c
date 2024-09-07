#include "myhdr.h"

int main(int argc,char *argv[])
{
    char *ar[10] = {"ping","134.208.6.61","-c10",NULL} ; // declaration and                                    initialization of an array
    
    execvp("ping",ar) ;
    
    printf ( " It does not get any response as execvp () function executes first \n ");
    return 0 ;
}