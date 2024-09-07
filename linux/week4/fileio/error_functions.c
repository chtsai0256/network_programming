/* error_functions.c
   Some standard error handling routines used by various programs.
*/
#include <stdarg.h>
#include "myhdr.h"

void
errExit(const char *format, ...)
{
    va_list argList;
    char msg[100];
    va_start(argList, format);
    sprintf(msg, format, argList);
    va_end(argList);
    perror(msg);
    exit(EXIT_FAILURE);
}


