#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

void finish(MYSQL *con) {
    printf("%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv)
{
	MYSQL *con = mysql_init(NULL);
  mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
  mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

  if (con==NULL) {
    printf("%s\n", mysql_error(con));
    exit(1);
  }
	
  if (mysql_real_connect(con, "134.208.6.50", "np2023", "@np2023@", NULL,0,NULL,0)==NULL ) 
    finish(con);

  char sql[100];
  int uid = 1;

  sprintf(sql, "DELETE FROM np2023.user WHERE uid=%d", uid);
  printf("%s\n", sql);
  if (mysql_query(con, sql)) 
    finish(con);
  
  mysql_close(con);
  exit(0);
}
