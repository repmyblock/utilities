#ifndef DATADISTRICTTEMPORAL_H
#define DATADISTRICTTEMPORAL_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataDistrictCycle_ID;
    uint32_t DataHouse_ID;
    uint32_t DataDistrict_ID;
} DataDistrictTemporal;

typedef struct DistrictTemporalHashTable DistrictTemporalHashTable;
typedef RecordMatch DistrictTemporalMatch;
typedef RecordLookupResult DistrictTemporalLookupResult;
#define DISTRICTTEMPORAL_ERROR RECORD_ERROR
#define DISTRICTTEMPORAL_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICTTEMPORAL_FOUND RECORD_FOUND
#define DISTRICTTEMPORAL_DUPLICATE RECORD_DUPLICATE

int LoadDistrictTemporalIntoMemory3(MYSQL *, DataDistrictTemporal **, size_t *);
int LoadDistrictTemporalIntoMemory4(MYSQL *, DataDistrictTemporal **, size_t *, size_t);
#define DistrictTemporal_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadDistrictTemporalIntoMemory(...) DistrictTemporal_LOAD_SELECT(__VA_ARGS__, LoadDistrictTemporalIntoMemory4, LoadDistrictTemporalIntoMemory3)(__VA_ARGS__)
void PrintDistrictTemporal(const DataDistrictTemporal *);
int ParseDistrictTemporalQuery(char *, void *);
int ParseDistrictTemporalRecord(char *, void *);
DistrictTemporalHashTable *BuildDistrictTemporalHashTable(const DataDistrictTemporal *, size_t);
DistrictTemporalMatch FindDistrictTemporalID(const DistrictTemporalHashTable *, const DataDistrictTemporal *);
void FreeDistrictTemporalMatch(DistrictTemporalMatch *);
void FreeDistrictTemporalHashTable(DistrictTemporalHashTable *);
int QueryDistrictTemporalMemory(const void *, const void *);
int QueryDistrictTemporalDatabase(MYSQL *, const void *);

#endif
