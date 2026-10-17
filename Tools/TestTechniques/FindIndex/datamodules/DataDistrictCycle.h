#ifndef DATA_DISTRICT_CYCLE_H
#define DATA_DISTRICT_CYCLE_H
#include "RecordHash.h"
#include <mysql/mysql.h>
typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    char CycleStartDate[11];
    char CycleEndDate[11];
} DataDistrictCycle;
enum {
    DISTRICT_CYCLE_FIELD_CYCLESTARTDATE = 1,
    DISTRICT_CYCLE_FIELD_CYCLEENDDATE = 2
};
typedef struct DistrictCycleHashTable DistrictCycleHashTable;
typedef RecordLookupResult DistrictCycleLookupResult;
typedef RecordMatch DistrictCycleMatch;
#define DISTRICT_CYCLE_ERROR RECORD_ERROR
#define DISTRICT_CYCLE_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICT_CYCLE_FOUND RECORD_FOUND
#define DISTRICT_CYCLE_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
DistrictCycleHashTable *BuildDistrictCycleHashTable(const DataDistrictCycle *, size_t);
DistrictCycleMatch FindDistrictCycleID(const DistrictCycleHashTable *, const DataDistrictCycle *);
void FreeDistrictCycleMatch(DistrictCycleMatch *);
void FreeDistrictCycleHashTable(DistrictCycleHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadDistrictCycleIntoMemory(MYSQL *, DataDistrictCycle **, size_t *);

int QueryDistrictCycleMemory(
    const void *hashTable,
    const void *record);

int QueryDistrictCycleDatabase(
    MYSQL *db,
    const void *record);

#endif
