#include <sys/wait.h>
#include <fcntl.h>
#include "myhdr.h"
#define OUT 1
#define IN 0

int main() {
	
	int exit_status;
	
    // 0 is read end, 1 is write end
    pid_t p1,p2;
	int fd[2];
	char *arg[50], *arg1[10];

	arg[0]="./number";
	arg[1]=NULL;

    arg1[0]="/bin/ls";
    arg1[1]=NULL;

    
	if (pipe(fd)<0) {
        perror("pipe");
        exit(EXIT_FAILURE);
     }
    

    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return 1;
    }
  
    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(fd[OUT]);
        dup2(fd[IN], STDIN_FILENO);
        close(fd[IN]);

        if (execvp(arg[0], arg) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } 

    p2 = fork();
    if (p2 < 0) {
        printf("\nCould not fork");
        return 1;
    }
    if (p2==0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(fd[IN]);
        dup2(fd[OUT], STDOUT_FILENO);
        close(fd[OUT]);

        if (execvp(arg1[0], arg1) < 0) {
            printf("\nCould not execute command 2..");
            exit(0);
        }
    }

    int status;
    // Parent executing
    waitpid(p2, &status, WUNTRACED);
    waitpid(p1, &status, WUNTRACED);
    printf("OK\n");
	return 0;
}
