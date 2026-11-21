#ifndef DATASTREET_H
#define DATASTREET_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
} DataStreet;

typedef struct StreetHashTable StreetHashTable;
typedef RecordMatch StreetMatch;
typedef RecordLookupResult StreetLookupResult;
#define STREET_ERROR RECORD_ERROR
#define STREET_NOT_FOUND RECORD_NOT_FOUND
#define STREET_FOUND RECORD_FOUND
#define STREET_DUPLICATE RECORD_DUPLICATE

int LoadStreetIntoMemory3(MYSQL *, DataStreet **, size_t *);
int LoadStreetIntoMemory4(MYSQL *, DataStreet **, size_t *, size_t);
#define Street_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadStreetIntoMemory(...) Street_LOAD_SELECT(__VA_ARGS__, LoadStreetIntoMemory4, LoadStreetIntoMemory3)(__VA_ARGS__)
void PrintStreet(const DataStreet *);
int ParseStreetQuery(char *, void *);
int ParseStreetRecord(char *, void *);
StreetHashTable *BuildStreetHashTable(const DataStreet *, size_t);
StreetMatch FindStreetID(const StreetHashTable *, const DataStreet *);
void FreeStreetMatch(StreetMatch *);
void FreeStreetHashTable(StreetHashTable *);
int QueryStreetMemory(const void *, const void *);
int QueryStreetDatabase(MYSQL *, const void *);

#endif
