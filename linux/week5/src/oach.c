#include <signal.h>
#include "myhdr.h"

static void
sigHandler(int sig) {
     printf("Ouch!\n"); 
     _exit();
}
int
main(int argc, char *argv[])
{
 int j;
 if (signal(SIGINT, sigHandler) == SIG_ERR)
    perror("signal");
 
 for (j = 0; ; j++) {
     printf("%d\n", j);
     sleep(3); 
 }
}