/* seek_io.c

   Demonstrate the use of lseek() and file I/O system calls.

   Usage: seek_io file {r<length>|R<length>|w<string>|s<offset>}...

   This program opens the file named on its command line, and then performs
   the file I/O operations specified by its remaining command-line arguments:

           r<length>    Read 'length' bytes from the file at current
                        file offset, displaying them as text.

           R<length>    Read 'length' bytes from the file at current
                        file offset, displaying them in hex.

           w<string>    Write 'string' at current file offset.

           s<offset>    Set the file offset to 'offset'.

   Example:

        seek_io myfile wxyz s1 r2
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "myhdr.h"

int
main(int argc, char *argv[])
{
    size_t len;
    off_t offset;
    int fd, ap, j;
    unsigned char *buf;
    ssize_t numRead, numWritten;
    char *endptr;

    //TRACE_LOG("argc = %d\n", argc);

    if (argc < 3) 
        errExit("%s file {r<length>|s<offset>}...\n", argv[0]);

    fd = open(argv[1], O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd == -1) 
        errExit("open");

    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':   /* Display bytes at current offset, as text */
            len = strtol(&argv[ap][1], &endptr, 10);
            printf("len=%ld\n",len);

            buf = malloc(len);
            if (buf == NULL)
                errExit("malloc");

            numRead = read(fd, buf, len);
            if (numRead == -1)
                errExit("read");

            if (numRead == 0) {
                printf("%s: end-of-file\n", argv[ap]);
            } else {
                printf("%s: ", argv[ap]);
                for (j = 0; j < numRead; j++) {
                    printf("%c", isprint(buf[j]) ?  buf[j] : '?');
                }
                printf("\n");
            }

            free(buf);
            break;

        case 'w':   /* Write string at current offset */
            numWritten = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (numWritten == -1)
                errExit("write");
            printf("%s: wrote %ld bytes\n", argv[ap], (long) numWritten);
            break;

        case 's': /* Change file offset */
            offset = strtol(&argv[ap][1],&endptr, 10);
            printf("offset=%ld\n",offset);
            if (lseek(fd, offset, SEEK_SET) == -1)
                errExit("lseek");
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            printf("Argument must start with [rw]: %s\n", argv[ap]);
        }
    }

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
