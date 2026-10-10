#include <stdio.h>
#include <mysql/mysql.h>

// void main(void) {	
//	printf("Test\n");
// }

int main(void) {
  MYSQL *db = mysql_init(NULL);
  if (db == NULL) {
    fprintf(stderr, "Could not initialize MySQL\n");
    return 1;
  }

  if (mysql_real_connect(db, "192.168.199.18", "usracct", "usracct", "RepMyBlock", 3306, NULL, 0) == NULL) {
    fprintf(stderr, "Connection failed: %s\n", mysql_error(db));
    mysql_close(db);
    return 1;
  }

  if (mysql_query(db, "SELECT * FROM DataFirstName")) {
    fprintf(stderr, "Query failed: %s\n", mysql_error(db));
    mysql_close(db);
    return 1;
  }

  MYSQL_RES *result = mysql_store_result(db);
  if (result == NULL) {
    fprintf(stderr, "Result failed: %s\n", mysql_error(db));
    mysql_close(db);
    return 1;
  }

  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result)) != NULL) {
	  printf("ID: %s | Name: %s\n", row[0] ? row[0] : "NULL", row[1] ? row[1] : "NULL");
  }

  mysql_free_result(result);
  mysql_close(db);
  return 0;
}