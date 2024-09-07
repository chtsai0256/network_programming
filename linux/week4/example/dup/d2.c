#include <sys/stat.h>
#include "myhdr.h"

int main(void) {
  int number1, number2, sum;
  int input_fds = open("./input.txt", O_RDONLY);
  int output_fds = open("./output.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  dup2(input_fds, STDIN_FILENO);
  dup2(output_fds, STDOUT_FILENO);
  
  scanf("%d %d", &number1, &number2);
  sum = number1 + number2;
  
  printf("%d + %d = %d\n", number1, number2, sum);
  return EXIT_SUCCESS;
}