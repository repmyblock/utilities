#ifndef DATA_COUNTY_H
#define DATA_COUNTY_H
#include "RecordHash.h"

typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t DataState_ID;
    char Name[161];
    uint32_t BOEID;
    uint32_t GEOID;
} DataCounty;
enum {
    COUNTY_FIELD_DATASTATE_ID = 1,
    COUNTY_FIELD_NAME = 2,
    COUNTY_FIELD_BOEID = 3,
    COUNTY_FIELD_GEOID = 4
};
typedef struct CountyHashTable CountyHashTable;
typedef RecordLookupResult CountyLookupResult;
typedef RecordMatch CountyMatch;
#define COUNTY_ERROR RECORD_ERROR
#define COUNTY_NOT_FOUND RECORD_NOT_FOUND
#define COUNTY_FOUND RECORD_FOUND
#define COUNTY_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
CountyHashTable *BuildCountyHashTable(const DataCounty *, size_t);
CountyMatch FindCountyID(const CountyHashTable *, const DataCounty *);
void FreeCountyMatch(CountyMatch *);
void FreeCountyHashTable(CountyHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadCountyIntoMemory(MYSQL *, DataCounty **, size_t *);


int QueryCountyMemory(const void *hashTable, const void *record);
int QueryCountyDatabase(MYSQL *db, const void *record);

#endif
