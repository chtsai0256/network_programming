#include <sys/wait.h>
#include "myhdr.h"
#include <string.h>

#define READ_END 0
#define WRITE_END 1


pid_t pipe_open(int pipefd[], char rw, char *exeArgv[])
{
    pid_t cpid;
    cpid = fork();
    switch(cpid) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0:
            if (rw == 'w') {
                close(pipefd[READ_END]);
                dup2(pipefd[WRITE_END], STDOUT_FILENO);
                close(pipefd[WRITE_END]);
            } else {
                close(pipefd[WRITE_END]);
                dup2(pipefd[READ_END], STDIN_FILENO);
                close(pipefd[READ_END]);
            }
            if (execvp(exeArgv[0], exeArgv) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            };
            exit(EXIT_FAILURE);
            break;
        default:
            return cpid;
            break;
    }
}

int
main(int argc, char *argv[])
{
    int fd0[2], fd1[2];
    char buf;
    char *exeArgv0[50];
    char *exeArgv1[50];

    exeArgv0[0] = argv[1];
    exeArgv0[1] = NULL;

    exeArgv1[0] = argv[2];
    exeArgv1[1] = NULL;
    

    if ( (pipe(fd0) == -1) || (pipe(fd1)==-1) )  {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid_in = pipe_open(fd0, 'w', exeArgv0); //output to parent
    pid_t pid_to = pipe_open(fd1, 'r', exeArgv1); //input from parent
    
    int numReads = read(fd0[READ_END], &buf, 1);

    int status;
    while (numReads > 0) {
        printf("%c",buf);
        write(fd1[WRITE_END], &buf, 1);
        if (waitpid(pid_in, &status, WUNTRACED)<0);
            numReads = read(fd0[READ_END], &buf, 1);
    }
    //buf = EOF;
    write(fd1[WRITE_END], &buf, 1);
    close(fd0[READ_END]);
    close(fd1[WRITE_END]);          /* Reader will see EOF */
    waitpid(pid_in, &status, WUNTRACED);                /* Wait for child */
    waitpid(pid_to, &status, WUNTRACED);                /* Wait for child */
    exit(EXIT_SUCCESS);
}               