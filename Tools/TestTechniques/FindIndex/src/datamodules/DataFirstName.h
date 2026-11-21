#ifndef DATAFIRSTNAME_H
#define DATAFIRSTNAME_H

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
} DataFirstName;

typedef struct FirstNameHashTable FirstNameHashTable;
typedef RecordMatch FirstNameMatch;
typedef RecordLookupResult FirstNameLookupResult;
#define FIRSTNAME_ERROR RECORD_ERROR
#define FIRSTNAME_NOT_FOUND RECORD_NOT_FOUND
#define FIRSTNAME_FOUND RECORD_FOUND
#define FIRSTNAME_DUPLICATE RECORD_DUPLICATE

int LoadFirstNameIntoMemory3(MYSQL *, DataFirstName **, size_t *);
int LoadFirstNameIntoMemory4(MYSQL *, DataFirstName **, size_t *, size_t);
#define FirstName_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadFirstNameIntoMemory(...) FirstName_LOAD_SELECT(__VA_ARGS__, LoadFirstNameIntoMemory4, LoadFirstNameIntoMemory3)(__VA_ARGS__)
void PrintFirstName(const DataFirstName *);
int ParseFirstNameQuery(char *, void *);
int ParseFirstNameRecord(char *, void *);
FirstNameHashTable *BuildFirstNameHashTable(const DataFirstName *, size_t);
FirstNameMatch FindFirstNameID(const FirstNameHashTable *, const DataFirstName *);
void FreeFirstNameMatch(FirstNameMatch *);
void FreeFirstNameHashTable(FirstNameHashTable *);
int QueryFirstNameMemory(const void *, const void *);
int QueryFirstNameDatabase(MYSQL *, const void *);

#endif
