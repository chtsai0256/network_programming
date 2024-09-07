#include <sys/wait.h>
#include <time.h>
#include "myhdr.h"

int
main(int argc, char *argv[])
{
    int numDead; /* Number of children so far waited for */
    pid_t childPid; /* PID of waited for child */
    int j;
    time_t t;

 
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s 10, 20, ...\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    
    setbuf(stdout, NULL); /* Disable buffering of stdout */
 
    for (j = 1; j < argc; j++) { /* Create one child for each argument */
        switch (fork()) {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0: /* Child sleeps for a while then exits */
                time(&t);
                printf("[%s] child %d started with PID %ld, sleeping %s seconds\n", 
                        ctime(&t), j, (long) getpid(), argv[j]);

                sleep(atoi(argv[j]));
                exit(EXIT_SUCCESS);
            default: /* Parent just continues around loop */
                break;
        }
    }
 
    numDead = 0;
    int status;
    for (;;) { /* Parent waits for each child to exit */
        childPid = waitpid( -1, &status, WNOHANG); //WUNTRACED );
        //WNOHANG

        if (childPid == -1) {
            if (errno == ECHILD) {
                printf("No more children - bye!\n");
                exit(EXIT_SUCCESS);
            } else { /* Some other (unexpected) error */
                perror("wait\n");
                exit(EXIT_FAILURE);
            }
        }
        numDead++;
        time(&t);
        printf("[%s] wait() returned child PID %ld (numDead=%d)\n",
                ctime(&t), (long) childPid, numDead);
    }
}
