#ifndef DATAINTER_H
#define DATAINTER_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
    char Abbrev[161];
    char Web[1021];
    char LogoPath[1021];
    uint32_t Left_ID;
    uint32_t Right_ID;
    char DBTable[1025];
    char ValidFrom[40];
    char ValidTo[40];
} DataInter;

typedef struct InterHashTable InterHashTable;
typedef RecordMatch InterMatch;
typedef RecordLookupResult InterLookupResult;
#define INTER_ERROR RECORD_ERROR
#define INTER_NOT_FOUND RECORD_NOT_FOUND
#define INTER_FOUND RECORD_FOUND
#define INTER_DUPLICATE RECORD_DUPLICATE

int LoadInterIntoMemory3(MYSQL *, DataInter **, size_t *);
int LoadInterIntoMemory4(MYSQL *, DataInter **, size_t *, size_t);
#define Inter_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadInterIntoMemory(...) Inter_LOAD_SELECT(__VA_ARGS__, LoadInterIntoMemory4, LoadInterIntoMemory3)(__VA_ARGS__)
void PrintInter(const DataInter *);
int ParseInterQuery(char *, void *);
int ParseInterRecord(char *, void *);
InterHashTable *BuildInterHashTable(const DataInter *, size_t);
InterMatch FindInterID(const InterHashTable *, const DataInter *);
void FreeInterMatch(InterMatch *);
void FreeInterHashTable(InterHashTable *);
int QueryInterMemory(const void *, const void *);
int QueryInterDatabase(MYSQL *, const void *);

#endif
