#ifndef DATALASTNAME_H
#define DATALASTNAME_H

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
} DataLastName;

typedef struct LastNameHashTable LastNameHashTable;
typedef RecordMatch LastNameMatch;
typedef RecordLookupResult LastNameLookupResult;
#define LASTNAME_ERROR RECORD_ERROR
#define LASTNAME_NOT_FOUND RECORD_NOT_FOUND
#define LASTNAME_FOUND RECORD_FOUND
#define LASTNAME_DUPLICATE RECORD_DUPLICATE

int LoadLastNameIntoMemory3(MYSQL *, DataLastName **, size_t *);
int LoadLastNameIntoMemory4(MYSQL *, DataLastName **, size_t *, size_t);
#define LastName_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadLastNameIntoMemory(...) LastName_LOAD_SELECT(__VA_ARGS__, LoadLastNameIntoMemory4, LoadLastNameIntoMemory3)(__VA_ARGS__)
void PrintLastName(const DataLastName *);
int ParseLastNameQuery(char *, void *);
int ParseLastNameRecord(char *, void *);
LastNameHashTable *BuildLastNameHashTable(const DataLastName *, size_t);
LastNameMatch FindLastNameID(const LastNameHashTable *, const DataLastName *);
void FreeLastNameMatch(LastNameMatch *);
void FreeLastNameHashTable(LastNameHashTable *);
int QueryLastNameMemory(const void *, const void *);
int QueryLastNameDatabase(MYSQL *, const void *);

#endif
