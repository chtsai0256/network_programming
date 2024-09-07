#include "myhdr.h"

int main() {
	
	int exit_status;
	
	char *arg[50];

	arg[0]="/bin/ls";
	arg[1]="-l";
	arg[2]=NULL;

    int fd;

       
    if (execvp(arg[0], arg) < 0) {
            printf("\nCould not execute command 1..");
            exit(EXIT_FAILURE);
    }
/*
    while(1) {
        sleep(5);
        printf("sleep!\n");
    };
*/
	return 0;
}
