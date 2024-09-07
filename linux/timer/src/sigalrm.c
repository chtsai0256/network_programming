#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

extern unsigned int ticks;

static void sig_handler(int sig){
	if (sig == SIGALRM) {
		ticks++;
		printf("ticks=%d\n",ticks);
		decrease_time();
		checkTimerList();
		alarm(1);
	}
}

int getTicks() {
	return ticks;
}

int start() {
	struct sigaction sa;
	bzero(&sa, sizeof(sa));

	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;
	assert(sigaction(SIGALRM, &sa, NULL)!=-1);
	alarm(1);
	while(1){
		//printf("+1s...\n");
		sleep(1);
	}
}

