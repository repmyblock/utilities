#include "CommonFiles.h"

MYSQL *DealDB(void) {
  MYSQL *db = mysql_init(NULL);

  if (db == NULL) {
    fprintf(stderr, "MySQL initialization failed.\n");
    return NULL;
  }

  if (mysql_options(db, MYSQL_READ_DEFAULT_GROUP, "RepMyBlockDev") != 0) {
    fprintf(stderr, "Could not configure login group: %s\n", mysql_error(db));
    mysql_close(db);
    return NULL;
  }

  /*
   * NULL credentials and port 0 allow the option-file
   * settings to supply host, user, password, and port.
   */
  if (mysql_real_connect(db, NULL, NULL, NULL, "RepMyBlock", 0, NULL, 0) == NULL) {
    fprintf(stderr, "Connection failed: %s\n", mysql_error(db));
    mysql_close(db);
    return NULL;
  }

  return db;
}

double NowMilliseconds(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    perror("clock_gettime");
    exit(EXIT_FAILURE);
  }
  return (double)ts.tv_sec * 1000.0  + (double)ts.tv_nsec / 1000000.0;
}

void PrintModuleLoad(const char *label, size_t count, size_t recordSize, double elapsedMs) {
  uint64_t totalMs=(uint64_t)(elapsedMs+0.5);
  printf("Loaded and indexed %zu %s in %.3f ms (or %" PRIu64 " hours %" PRIu64
         " minutes and %" PRIu64 " seconds and %" PRIu64
         " ms) - %.2f MiB of source records\n",
         count, label, elapsedMs, totalMs/3600000, (totalMs/60000)%60,
         (totalMs/1000)%60, totalMs%1000,
         (double)count*recordSize/(1024.0*1024.0));
}
