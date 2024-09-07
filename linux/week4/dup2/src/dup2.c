#include <sys/wait.h>
#include <fcntl.h>
#include "myhdr.h"

int main() {
	
	int exit_status;
	
    // 0 is read end, 1 is write end
    pid_t p1;
	int fd;
	char *arg[50];

	arg[0]="/bin/ls";
	arg[1]="-l";
	arg[2]=NULL;

	fd = open("test.txt", O_CREAT|O_WRONLY|O_TRUNC, 0600);

    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return 1;
    }
  
    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        dup2(fd, STDOUT_FILENO);

        if (execvp(arg[0], arg) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        wait(NULL);
    }
	return 0;
}
