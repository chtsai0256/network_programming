/* copy.c
   Copy the file named argv[1] to a new file named in argv[2].
*/
#include <sys/stat.h>
#include <fcntl.h>
#include "myhdr.h"


#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];


    /* Open input and output files */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        printf("%s:opening file %s\n", strerror(errno), argv[1]);
        exit(EXIT_FAILURE);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */


    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        printf("%s: opening file %s\n", strerror(errno), argv[2]);
        exit(EXIT_FAILURE);
    }

    printf("fd1=%d, fd2=%d\n", inputFd, outputFd);

    /* Transfer data until we encounter end of input or an error */
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
        if (write(outputFd, buf, numRead) != numRead)
            perror("write() returned error or partial write occurred");

    if (numRead == -1) 
        perror("read");

    //sleep(20);

    if (close(inputFd) == -1) 
        perror("close input");
    if (close(outputFd) == -1)
        perror("close output");

    /*
    while(1){
        
    }
    */
    exit(EXIT_SUCCESS);
}
