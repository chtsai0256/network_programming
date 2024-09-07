#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

static void sig_handler(int sig) {
	int retval;

	if (sig== SIGCHLD) {
		wait(&retval);
		printf("CATCH SIGNAL PID=%d\n", getpid());
		//exit(0);
	}
}

int main() {
	
	int exit_status;
	
	signal(SIGCHLD, sig_handler);

	pid_t PID = fork();	


	switch(PID) {
		case -1:
			break;
		case 0:
			printf("I am child\n");
			printf("C PID = %d\n", getpid());
			sleep(2);
			while(1){
				sleep(1);
			}
			break;
		default:
			printf("I am parent process\n");
			printf("p, PID = %d\n",getpid());
			wait(&exit_status);
			printf("[P] Child's exit status is [%d]\n", WEXITSTATUS(exit_status));
			while(1) {
				printf("P is busy\n");
				sleep(1);
			}
	}
	return 0;
}
