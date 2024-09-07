#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	pid_t PID = fork();

	switch(PID) {
		case -1:
			break;
		case 0:
			printf("I am child\n");
			printf("C PID = %d", getpid());
			sleep(10);
			break;
		default:
			printf("I am parent process\n");
			printf("p, PID = %d\n",getpid());
	}
}
