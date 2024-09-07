#include "../include/tools.h"

pipeTable pTable;

// static void sigChildHandler(int sig)
// {
//     int savedErrno;
//     savedErrno = errno;

//     while( waitpid(-1, NULL, WNOHANG) > 0 )
//         continue;
    
//     errno = savedErrno;
// }

// static void prepareSig()
// {
//     struct sigaction sa;

//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sa.sa_handler = sigChildHandler;
//     if(sigaction(SIGCHLD, &sa, NULL) == -1)
//         printf("Error for signal setting !\n");
// }

int main()
{
    // prepareSig();
    init();
    shellLoop();
    return 0;
}