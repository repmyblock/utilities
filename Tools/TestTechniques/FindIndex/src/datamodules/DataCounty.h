#ifndef DATACOUNTY_H
#define DATACOUNTY_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataState_ID;
    char Name[161];
    uint32_t BOEID;
    uint32_t GEOID;
} DataCounty;

typedef struct CountyHashTable CountyHashTable;
typedef RecordMatch CountyMatch;
typedef RecordLookupResult CountyLookupResult;
#define COUNTY_ERROR RECORD_ERROR
#define COUNTY_NOT_FOUND RECORD_NOT_FOUND
#define COUNTY_FOUND RECORD_FOUND
#define COUNTY_DUPLICATE RECORD_DUPLICATE

int LoadCountyIntoMemory3(MYSQL *, DataCounty **, size_t *);
int LoadCountyIntoMemory4(MYSQL *, DataCounty **, size_t *, size_t);
#define County_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadCountyIntoMemory(...) County_LOAD_SELECT(__VA_ARGS__, LoadCountyIntoMemory4, LoadCountyIntoMemory3)(__VA_ARGS__)
void PrintCounty(const DataCounty *);
int ParseCountyQuery(char *, void *);
int ParseCountyRecord(char *, void *);
CountyHashTable *BuildCountyHashTable(const DataCounty *, size_t);
CountyMatch FindCountyID(const CountyHashTable *, const DataCounty *);
void FreeCountyMatch(CountyMatch *);
void FreeCountyHashTable(CountyHashTable *);
int QueryCountyMemory(const void *, const void *);
int QueryCountyDatabase(MYSQL *, const void *);

#endif
