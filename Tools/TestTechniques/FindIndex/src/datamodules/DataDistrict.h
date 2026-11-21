#ifndef DATADISTRICT_H
#define DATADISTRICT_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char ZipCode[21];
    char DBTable[401];
    char DBTableValue[401];
    uint32_t DataCounty_ID;
    uint32_t DataDistrictTown_ID;
    uint32_t Electoral;
    uint32_t StateAssembly;
    uint32_t StateSenate;
    uint32_t Legislative;
    char Ward[13];
    uint32_t Congress;
} DataDistrict;

typedef struct DistrictHashTable DistrictHashTable;
typedef RecordMatch DistrictMatch;
typedef RecordLookupResult DistrictLookupResult;
#define DISTRICT_ERROR RECORD_ERROR
#define DISTRICT_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICT_FOUND RECORD_FOUND
#define DISTRICT_DUPLICATE RECORD_DUPLICATE

int LoadDistrictIntoMemory3(MYSQL *, DataDistrict **, size_t *);
int LoadDistrictIntoMemory4(MYSQL *, DataDistrict **, size_t *, size_t);
#define District_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadDistrictIntoMemory(...) District_LOAD_SELECT(__VA_ARGS__, LoadDistrictIntoMemory4, LoadDistrictIntoMemory3)(__VA_ARGS__)
void PrintDistrict(const DataDistrict *);
int ParseDistrictQuery(char *, void *);
int ParseDistrictRecord(char *, void *);
DistrictHashTable *BuildDistrictHashTable(const DataDistrict *, size_t);
DistrictMatch FindDistrictID(const DistrictHashTable *, const DataDistrict *);
void FreeDistrictMatch(DistrictMatch *);
void FreeDistrictHashTable(DistrictHashTable *);
int QueryDistrictMemory(const void *, const void *);
int QueryDistrictDatabase(MYSQL *, const void *);

#endif
