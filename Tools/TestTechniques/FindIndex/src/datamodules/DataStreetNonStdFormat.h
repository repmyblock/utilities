#ifndef DATASTREETNONSTDFORMAT_H
#define DATASTREETNONSTDFORMAT_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Text[1001];
} DataStreetNonStdFormat;

typedef struct StreetNonStdFormatHashTable StreetNonStdFormatHashTable;
typedef RecordMatch StreetNonStdFormatMatch;
typedef RecordLookupResult StreetNonStdFormatLookupResult;
#define STREETNONSTDFORMAT_ERROR RECORD_ERROR
#define STREETNONSTDFORMAT_NOT_FOUND RECORD_NOT_FOUND
#define STREETNONSTDFORMAT_FOUND RECORD_FOUND
#define STREETNONSTDFORMAT_DUPLICATE RECORD_DUPLICATE

int LoadStreetNonStdFormatIntoMemory3(MYSQL *, DataStreetNonStdFormat **, size_t *);
int LoadStreetNonStdFormatIntoMemory4(MYSQL *, DataStreetNonStdFormat **, size_t *, size_t);
#define StreetNonStdFormat_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadStreetNonStdFormatIntoMemory(...) StreetNonStdFormat_LOAD_SELECT(__VA_ARGS__, LoadStreetNonStdFormatIntoMemory4, LoadStreetNonStdFormatIntoMemory3)(__VA_ARGS__)
void PrintStreetNonStdFormat(const DataStreetNonStdFormat *);
int ParseStreetNonStdFormatQuery(char *, void *);
int ParseStreetNonStdFormatRecord(char *, void *);
StreetNonStdFormatHashTable *BuildStreetNonStdFormatHashTable(const DataStreetNonStdFormat *, size_t);
StreetNonStdFormatMatch FindStreetNonStdFormatID(const StreetNonStdFormatHashTable *, const DataStreetNonStdFormat *);
void FreeStreetNonStdFormatMatch(StreetNonStdFormatMatch *);
void FreeStreetNonStdFormatHashTable(StreetNonStdFormatHashTable *);
int QueryStreetNonStdFormatMemory(const void *, const void *);
int QueryStreetNonStdFormatDatabase(MYSQL *, const void *);

#endif
