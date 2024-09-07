#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
    pid_t childpid;
    int status;

    childpid = fork();

    if ( -1 == childpid )
    {
        perror( "fork()" );
        exit( EXIT_FAILURE );
    }
    else if ( 0 == childpid )
    {
        puts( "In child process" );
        sleep( 3 );//讓子進程睡眠3秒，看看父進程的行爲
        printf("\tchild pid = %d\n", getpid());
        printf("\tchild ppid = %d\n", getppid());
        exit(EXIT_SUCCESS);
    }
    else 
    {
        waitpid( childpid, &status, 0 );
        puts( "in parent" );
        printf( "\tparent pid = %d\n", getpid() );
        printf( "\tparent ppid = %d\n", getppid()  );
        printf( "\tchild process exited with status %d \n", status );
    }
    exit(EXIT_SUCCESS);
}