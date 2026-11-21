#ifndef DATADISTRICTCYCLE_H
#define DATADISTRICTCYCLE_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char CycleStartDate[16];
    char CycleEndDate[16];
} DataDistrictCycle;

typedef struct DistrictCycleHashTable DistrictCycleHashTable;
typedef RecordMatch DistrictCycleMatch;
typedef RecordLookupResult DistrictCycleLookupResult;
#define DISTRICTCYCLE_ERROR RECORD_ERROR
#define DISTRICTCYCLE_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICTCYCLE_FOUND RECORD_FOUND
#define DISTRICTCYCLE_DUPLICATE RECORD_DUPLICATE

int LoadDistrictCycleIntoMemory3(MYSQL *, DataDistrictCycle **, size_t *);
int LoadDistrictCycleIntoMemory4(MYSQL *, DataDistrictCycle **, size_t *, size_t);
#define DistrictCycle_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadDistrictCycleIntoMemory(...) DistrictCycle_LOAD_SELECT(__VA_ARGS__, LoadDistrictCycleIntoMemory4, LoadDistrictCycleIntoMemory3)(__VA_ARGS__)
void PrintDistrictCycle(const DataDistrictCycle *);
int ParseDistrictCycleQuery(char *, void *);
int ParseDistrictCycleRecord(char *, void *);
DistrictCycleHashTable *BuildDistrictCycleHashTable(const DataDistrictCycle *, size_t);
DistrictCycleMatch FindDistrictCycleID(const DistrictCycleHashTable *, const DataDistrictCycle *);
void FreeDistrictCycleMatch(DistrictCycleMatch *);
void FreeDistrictCycleHashTable(DistrictCycleHashTable *);
int QueryDistrictCycleMemory(const void *, const void *);
int QueryDistrictCycleDatabase(MYSQL *, const void *);

#endif
