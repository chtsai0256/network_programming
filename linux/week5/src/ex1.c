#include <sys/wait.h>
#include "myhdr.h"


#define READ_END 0
#define WRITE_END 1

int
main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    char b[1024]={0};

    scanf("%s", b);
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Child reads from pipe */
        close(pipefd[WRITE_END]);          /* Close unused write end */

        while (read(pipefd[READ_END], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[READ_END]);
        _exit(EXIT_SUCCESS);

    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd[READ_END]);          /* Close unused read end */
        write(pipefd[WRITE_END], b, strlen(b));
        close(pipefd[WRITE_END]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}               