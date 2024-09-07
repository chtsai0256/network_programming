#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main ()
{
    char buffer[1024] = { 0 };
    int len; 
    int pfd[2];
    int status;
    pid_t pid;

    /* create pipe */
    if (pipe(pfd)<0)
        return -1;

    /* fork to execute external program or scripts */
    pid = fork();
    if (pid<0) {
        return 0;
    } else if (pid==0) { /* child process */
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[1]);
        close(pfd[0]);

        /* execute CGI */
        if (execlp("ls", "ls", NULL)<0)
            perror("ls");
        exit(0);
    } else { /* parent process */
        int pid1=fork();

        if (pid1==0) {
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
            close(pfd[1]);
            if (execlp("./number", "number", NULL)<0)
                perror("number");
        }    
        wait(NULL);
        wait(NULL);
        while(1){
            printf("sleep\n");
            sleep(1);
        }
    }
}