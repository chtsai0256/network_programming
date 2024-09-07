/* 
   Standard header file used by nearly all of our example programs.
*/
#ifndef MY_HDR_H
#define MY_HDR_H      /* Prevent accidental double inclusion */

#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include <stdbool.h>    /* 'bool' type plus 'true' and 'false' constants */
#include <assert.h>
#include <signal.h>
#include <wait.h>
#include <fcntl.h>

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } Boolean;


typedef struct {
   pid_t pid;
   int fd;
} subprocess_t;

#define WRITE_END 1
#define READ_END 0

subprocess_t subprocess_in(char *commandArgv[]);

subprocess_t subprocess_out(char *commandArgv[]);
#endif
