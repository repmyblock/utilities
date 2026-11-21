#ifndef DATACITY_H
#define DATACITY_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
} DataCity;

typedef struct CityHashTable CityHashTable;
typedef RecordMatch CityMatch;
typedef RecordLookupResult CityLookupResult;
#define CITY_ERROR RECORD_ERROR
#define CITY_NOT_FOUND RECORD_NOT_FOUND
#define CITY_FOUND RECORD_FOUND
#define CITY_DUPLICATE RECORD_DUPLICATE

int LoadCityIntoMemory3(MYSQL *, DataCity **, size_t *);
int LoadCityIntoMemory4(MYSQL *, DataCity **, size_t *, size_t);
#define City_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadCityIntoMemory(...) City_LOAD_SELECT(__VA_ARGS__, LoadCityIntoMemory4, LoadCityIntoMemory3)(__VA_ARGS__)
void PrintCity(const DataCity *);
int ParseCityQuery(char *, void *);
int ParseCityRecord(char *, void *);
CityHashTable *BuildCityHashTable(const DataCity *, size_t);
CityMatch FindCityID(const CityHashTable *, const DataCity *);
void FreeCityMatch(CityMatch *);
void FreeCityHashTable(CityHashTable *);
int QueryCityMemory(const void *, const void *);
int QueryCityDatabase(MYSQL *, const void *);

#endif
