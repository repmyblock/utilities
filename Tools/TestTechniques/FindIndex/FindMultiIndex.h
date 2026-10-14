#include "datamodules/DataAddress.h"
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
#include <ctype.h>
#include <errno.h>

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

static int RunQueryLoops(
    MYSQL *db,
    const NameHashTable *firstNames,
    const NameHashTable *middleNames,
    const NameHashTable *lastNames,
    const AddressHashTable *addresses
);


/* Database functions */
MYSQL *DealDB(void);
int LoadDBintoMEM(MYSQL *, Person **, size_t *, char *);
int FindNameInDB(MYSQL *, const char *, const char *);
int FindNameInMEM(const Person *, size_t, const char *);
int CompareNameByName(const void *, const void *);
int BuildNameHashTable(NameHashTable *, const Person *, size_t);
size_t FindNameInHASH(const NameHashTable *, const char *);
void PrintHashMatches(const NameHashTable *, size_t, const char *);
void FreeNameHashTable(NameHashTable *);
static int ReadQueryLine(const char *, char *, size_t);
static int ParseAddressQuery(char *, DataAddress *);

#endif /* FIND_MULTIPLE_INDEX_H */
