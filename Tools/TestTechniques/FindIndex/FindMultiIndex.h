#ifndef FIND_MULTIPLE_INDEX_H
#define FIND_MULTIPLE_INDEX_H

#include "datamodules/RecordLoader.h"
#include "DataModules.h"

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

typedef int (*ParseRecordFunction)(char *, void *);
typedef int (*FindMemoryFunction)(const void *, const void *);
typedef int (*FindDatabaseFunction)(MYSQL *, const void *);
typedef void (*PrintRecordFunction)(const void *);

typedef struct {
    char key;
    const char *label;
    const char *prompt;
    const void *hashTable;
    size_t recordSize;
    ParseRecordFunction parse;
    FindMemoryFunction findMemory;
    FindDatabaseFunction findDatabase;
    PrintRecordFunction print;
} QueryDefinition;

MYSQL *DealDB(void);

int LoadDBintoMEM(
    MYSQL *,
    Person **,
    size_t *,
    char *);

int FindNameInDB(
    MYSQL *,
    const char *,
    const char *);

int FindNameInMEM(
    const Person *,
    size_t,
    const char *);

int CompareNameByName(const void *, const void *);

int BuildNameHashTable(
    NameHashTable *,
    const Person *,
    size_t);

size_t FindNameInHASH(
    const NameHashTable *,
    const char *);

void PrintHashMatches(
    const NameHashTable *,
    size_t,
    const char *);

void FreeNameHashTable(NameHashTable *);

static int ParseAddressRecord(char *line, void *record);

/*
	int RunQueryLoops(MYSQL *, const QueryDefinition *, size_t);
	int ReadQueryLine(const char *, char *, size_t);
*/
#endif