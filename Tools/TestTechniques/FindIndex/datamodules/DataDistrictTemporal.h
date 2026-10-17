#ifndef DATA_DISTRICT_TEMPORAL_H
#define DATA_DISTRICT_TEMPORAL_H
#include "RecordHash.h"

typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t DataDistrictCycle_ID;
    uint32_t DataHouse_ID;
    uint32_t DataDistrict_ID;
} DataDistrictTemporal;

enum {
    DISTRICT_TEMPORAL_FIELD_DATADISTRICTCYCLE_ID = 1,
    DISTRICT_TEMPORAL_FIELD_DATAHOUSE_ID = 2,
    DISTRICT_TEMPORAL_FIELD_DATADISTRICT_ID = 3
};

typedef struct DistrictTemporalHashTable DistrictTemporalHashTable;
typedef RecordLookupResult DistrictTemporalLookupResult;
typedef RecordMatch DistrictTemporalMatch;

#define DISTRICT_TEMPORAL_ERROR RECORD_ERROR
#define DISTRICT_TEMPORAL_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICT_TEMPORAL_FOUND RECORD_FOUND
#define DISTRICT_TEMPORAL_DUPLICATE RECORD_DUPLICATE

/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
DistrictTemporalHashTable *BuildDistrictTemporalHashTable(const DataDistrictTemporal *, size_t);
DistrictTemporalMatch FindDistrictTemporalID(const DistrictTemporalHashTable *, const DataDistrictTemporal *);
void FreeDistrictTemporalMatch(DistrictTemporalMatch *);
void FreeDistrictTemporalHashTable(DistrictTemporalHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadDistrictTemporalIntoMemory(MYSQL *, DataDistrictTemporal **, size_t *);


int QueryDistrictTemporalMemory(const void *hashTable,const void *record);
int QueryDistrictTemporalDatabase(MYSQL *db,const void *record);
#endif
