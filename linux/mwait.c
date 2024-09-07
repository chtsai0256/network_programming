#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	pid_t PID = fork();
	int exit_status;

	switch(PID) {
		case -1:
			break;
		case 0:
			printf("I am child\n");
			printf("C PID = %d\n", getpid());
			scanf("%d", &exit_status);
			sleep(10);
			break;
		default:
			printf("I am parent process\n");
			printf("p, PID = %d\n",getpid());
			wait(&exit_status);
			printf("[P] Child's exit status is [%d]\n", WEXITSTATUS(exit_status));
	}
	return 0;
}
