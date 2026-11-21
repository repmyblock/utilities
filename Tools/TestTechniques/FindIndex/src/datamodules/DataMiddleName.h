#ifndef DATAMIDDLENAME_H
#define DATAMIDDLENAME_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Text[1025];
    char Compress[1025];
} DataMiddleName;

typedef struct MiddleNameHashTable MiddleNameHashTable;
typedef RecordMatch MiddleNameMatch;
typedef RecordLookupResult MiddleNameLookupResult;
#define MIDDLENAME_ERROR RECORD_ERROR
#define MIDDLENAME_NOT_FOUND RECORD_NOT_FOUND
#define MIDDLENAME_FOUND RECORD_FOUND
#define MIDDLENAME_DUPLICATE RECORD_DUPLICATE

int LoadMiddleNameIntoMemory3(MYSQL *, DataMiddleName **, size_t *);
int LoadMiddleNameIntoMemory4(MYSQL *, DataMiddleName **, size_t *, size_t);
#define MiddleName_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadMiddleNameIntoMemory(...) MiddleName_LOAD_SELECT(__VA_ARGS__, LoadMiddleNameIntoMemory4, LoadMiddleNameIntoMemory3)(__VA_ARGS__)
void PrintMiddleName(const DataMiddleName *);
int ParseMiddleNameQuery(char *, void *);
int ParseMiddleNameRecord(char *, void *);
MiddleNameHashTable *BuildMiddleNameHashTable(const DataMiddleName *, size_t);
MiddleNameMatch FindMiddleNameID(const MiddleNameHashTable *, const DataMiddleName *);
void FreeMiddleNameMatch(MiddleNameMatch *);
void FreeMiddleNameHashTable(MiddleNameHashTable *);
int QueryMiddleNameMemory(const void *, const void *);
int QueryMiddleNameDatabase(MYSQL *, const void *);

#endif
