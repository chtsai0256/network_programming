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
    int pfd[2], pfd1[2];
    int status;
    pid_t pid;

    /* create pipe */
    if (pipe(pfd)<0)
        return -1;
    
    if (pipe(pfd1)<0)
        return -1;

    /* fork to execute external program or scripts */
    pid = fork();
    if (pid<0) {
        return 0;
    } else if (pid==0) { /* child process */
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);
        close(pfd[1]);
        close(pfd1[0]);
        close(pfd1[1]);

        execlp("ls", "ls", NULL);
        exit(0);
    } else { /* parent process */

        int pid1=fork();
        if (pid1<0) {
            return 0;
        } else if (pid1==0) { /* child process */
            dup2(pfd1[0], STDIN_FILENO);
            close(pfd[0]);
            close(pfd[1]);
            close(pfd1[0]);
            close(pfd1[1]);
            
            execlp("./number", "number", NULL);
            exit(0);
        }

        close(pfd[1]);
        close(pfd1[0]);
        /* print output from CGI */
        while((len=read(pfd[0], buffer, 1023))>0) {
            buffer[len] = '\0';
            len = write(pfd1[1], buffer, strlen(buffer)+1);
        }
        
        close(pfd[0]);
        close(pfd1[1]);
        waitpid((pid_t)pid, &status, 0);
        waitpid((pid_t) pid1, &status, 0);

    }
}