#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

static int flag=0;


static void sig_handler(int sig){
	if (sig == SIGALRM) {
		//puts("+1s...");
		flag=1;
		alarm(2);
	}
}


int main(){
	struct sigaction sa;
	bzero(&sa, sizeof(sa));

	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;

	assert(sigaction(SIGALRM, &sa, NULL)!=-1);

	alarm(1);

	while(1){
		if (flag) {
			puts("+1s..\n");
			flag=0;
		}
		printf("main loop delay\n");
		sleep(1);
	}
	return 0;
}
