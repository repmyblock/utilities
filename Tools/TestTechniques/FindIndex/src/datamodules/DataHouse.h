#ifndef DATAHOUSE_H
#define DATAHOUSE_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataAddress_ID;
    char Type[41];
    char Apt[401];
    uint32_t DataStreetNonStdFormat_ID;
    uint32_t BIN;
} DataHouse;

typedef struct HouseHashTable HouseHashTable;
typedef RecordMatch HouseMatch;
typedef RecordLookupResult HouseLookupResult;
#define HOUSE_ERROR RECORD_ERROR
#define HOUSE_NOT_FOUND RECORD_NOT_FOUND
#define HOUSE_FOUND RECORD_FOUND
#define HOUSE_DUPLICATE RECORD_DUPLICATE

int LoadHouseIntoMemory3(MYSQL *, DataHouse **, size_t *);
int LoadHouseIntoMemory4(MYSQL *, DataHouse **, size_t *, size_t);
#define House_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadHouseIntoMemory(...) House_LOAD_SELECT(__VA_ARGS__, LoadHouseIntoMemory4, LoadHouseIntoMemory3)(__VA_ARGS__)
void PrintHouse(const DataHouse *);
int ParseHouseQuery(char *, void *);
int ParseHouseRecord(char *, void *);
HouseHashTable *BuildHouseHashTable(const DataHouse *, size_t);
HouseMatch FindHouseID(const HouseHashTable *, const DataHouse *);
void FreeHouseMatch(HouseMatch *);
void FreeHouseHashTable(HouseHashTable *);
int QueryHouseMemory(const void *, const void *);
int QueryHouseDatabase(MYSQL *, const void *);

#endif
