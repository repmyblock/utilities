#ifndef FIND_MULTIPLE_INDEX_H
#define FIND_MULTIPLE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>


typedef struct {
    int id;
    char name[255];
} Person;

typedef struct {
    size_t *buckets;
    size_t *next;
    size_t bucketCount;
    size_t collisions;
    const Person *Data;
} NameHashTable;

/* Database functions */
MYSQL *DealDB(void);
int LoadDBintoMEM(MYSQL *, Person **, size_t *, char *);
int FindNameInDB(MYSQL *, const char *);
int FindNameInMEM(const Person *, size_t, const char *);
static double NowMilliseconds(void);
int CompareNameByName(const void *, const void *);
static uint64_t HashName(const char *);
int BuildNameHashTable(NameHashTable *, const Person *, size_t);
size_t FindNameInHASH(const NameHashTable *, const char *);
void PrintHashMatches(const NameHashTable *, size_t, const char *);
void FreeNameHashTable(NameHashTable *);

#endif /* FIND_MULTIPLE_INDEX_H */