#include <sys/wait.h>
#include <fcntl.h>
#include "myhdr.h"


#define OUT 1
#define IN 0

int main(int argc, char **argv)
{
  int pipefd[2];
  int pid;

  char *cat_args[] = {"", "", NULL};
  char *grep_args[] = {"ls", "-l", NULL};

  // make a pipe (fds go in pipefd[0] and pipefd[1])

  pipe(pipefd);

  pid = fork();

  if (pid == 0)
    {
      // child gets here and handles "grep Villanova"

      // replace standard input with input part of pipe

      dup2(pipefd[OUT], STDOUT_FILENO);

      // close unused hald of pipe

      close(pipefd[IN]);

      // execute grep

      execvp("ls", grep_args);
    }
  else
    {
      // parent gets here and handles "cat scores"

      // replace standard output with output part of pipe

      dup2(pipefd[IN], STDIN_FILENO);

      // close unused unput half of pipe

      close(pipefd[OUT]);

      // execute cat

      wait(NULL);
      execvp("more", cat_args);
    }
}
