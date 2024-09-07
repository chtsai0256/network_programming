#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  char *version = mysql_get_client_info();
  printf("MySQL client version: %s\n", version);
  exit(0);
}
