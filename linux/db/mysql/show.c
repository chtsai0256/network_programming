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
  
  sprintf(sql, "SELECT * FROM np2023.smart WHERE count>20 ORDER BY count desc limit 0,10");
  printf("%s\n", sql);
  if (mysql_query(con, sql)) 
    finish(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result==NULL)
    finish(con);

  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;

  while((row=mysql_fetch_row(result))) {
    for (int i=0;i<num_fields;i++) {
      printf("%s, ", row[i] ? row[i]:"NULL");
    }
    printf("\n");
  }
  
  mysql_free_result(result);
  mysql_close(con);
  exit(0);
}