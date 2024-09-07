#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
			execlp("/bin/ls","ls","-l");
    			printf("will this line be printed");
			sleep(5);
			break;
		default:
			printf("I am parent process\n");
			printf("p, PID = %d\n",getpid());
			//wait(&exit_status);
			printf("[P] Child's exit status is [%d]\n", WEXITSTATUS(exit_status));
			int i=0;
			while(i<10) {
				printf("P is busy\n");
				sleep(1);
				i++;
			}
	}
	return 0;
}
