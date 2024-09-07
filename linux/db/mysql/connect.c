#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con==NULL) {
      printf("%s\n", mysql_error(con));
      exit(1);
    }
	
    if (mysql_real_connect(con, "134.208.6.50", "np2023", "@np2023@", NULL,0,NULL,0)==NULL ) {
        printf("%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    printf("MySQL Server Connect Success!!\n");
    mysql_close(con);
    exit(0);
}
