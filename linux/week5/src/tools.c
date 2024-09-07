#include <sys/wait.h>
#include "myhdr.h"

void reapChild(int unused) {
    while(1) {
        pid_t pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0) break;
    }
}

subprocess_t subprocess_in(char *commandArgv[]) {
    int fds[2];
    if (pipe(fds)<0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    subprocess_t process = {fork(), fds[WRITE_END]};
    if(process.pid==0) {
        close(fds[WRITE_END]);
        dup2(fds[READ_END], STDIN_FILENO);
        close(fds[READ_END]);
        execvp(commandArgv[0], commandArgv);
    }
    close(fds[READ_END]);
    return process;
}

subprocess_t subprocess_out(char *commandArgv[]) {
    int fds[2];
    if (pipe(fds)<0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    subprocess_t process = {fork(), fds[READ_END]};
    if(process.pid==0) {
        close(fds[READ_END]);
        dup2(fds[WRITE_END], STDOUT_FILENO);
        close(fds[WRITE_END]);
        execvp(commandArgv[0], commandArgv);
    }
    close(fds[WRITE_END]);
    return process;
}