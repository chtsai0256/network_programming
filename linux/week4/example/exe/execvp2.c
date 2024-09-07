#include <sys/wait.h>
#include "myhdr.h"

int main()
{
    char *cmd = "ls" ;
    char *arg_ls[] = {"ls","-l",NULL};

    printf ( "Example of the execution of a statement before execvp() is invoked \n" ) ;
    printf ( "We are introducing one more process which indicates a fork \n" ) ;

    if( fork() == 0 ) //child process is introduced which is controlled by "ls-l"
    {
        int sts_cd = execvp(cmd,arg_ls) ;
        printf( "This statement executes that means there is an issue running the command \n" ) ;

        if( sts_cd == -1 )
        {
            printf( "execvp error! \n" ) ;
            return 1 ;
        }
    }
    else // parent program run here
    {
        wait(NULL);
        printf ( "This statement executes that means back to main program \n" ) ;
    }
    return 0 ;
}