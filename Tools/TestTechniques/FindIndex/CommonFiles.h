#ifndef COMMON_FILES_H
#define COMMON_FILES_H

#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include "RecordHash.h"
#include "DataModules.h"

MYSQL *DealDB(void);
double NowMilliseconds(void);
void PrintModuleLoad(const char *, size_t, size_t, double);

#endif
