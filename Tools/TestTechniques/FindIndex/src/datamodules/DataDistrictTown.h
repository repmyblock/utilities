#ifndef DATADISTRICTTOWN_H
#define DATADISTRICTTOWN_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
} DataDistrictTown;

typedef struct DistrictTownHashTable DistrictTownHashTable;
typedef RecordMatch DistrictTownMatch;
typedef RecordLookupResult DistrictTownLookupResult;
#define DISTRICTTOWN_ERROR RECORD_ERROR
#define DISTRICTTOWN_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICTTOWN_FOUND RECORD_FOUND
#define DISTRICTTOWN_DUPLICATE RECORD_DUPLICATE

int LoadDistrictTownIntoMemory3(MYSQL *, DataDistrictTown **, size_t *);
int LoadDistrictTownIntoMemory4(MYSQL *, DataDistrictTown **, size_t *, size_t);
#define DistrictTown_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadDistrictTownIntoMemory(...) DistrictTown_LOAD_SELECT(__VA_ARGS__, LoadDistrictTownIntoMemory4, LoadDistrictTownIntoMemory3)(__VA_ARGS__)
void PrintDistrictTown(const DataDistrictTown *);
int ParseDistrictTownQuery(char *, void *);
int ParseDistrictTownRecord(char *, void *);
DistrictTownHashTable *BuildDistrictTownHashTable(const DataDistrictTown *, size_t);
DistrictTownMatch FindDistrictTownID(const DistrictTownHashTable *, const DataDistrictTown *);
void FreeDistrictTownMatch(DistrictTownMatch *);
void FreeDistrictTownHashTable(DistrictTownHashTable *);
int QueryDistrictTownMemory(const void *, const void *);
int QueryDistrictTownDatabase(MYSQL *, const void *);

#endif
