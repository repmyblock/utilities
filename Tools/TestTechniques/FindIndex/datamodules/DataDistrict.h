#ifndef DATA_DISTRICT_H
#define DATA_DISTRICT_H
#include "RecordHash.h"

typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    char ZipCode[21];
    char DBTable[401];
    char DBTableValue[401];
    uint32_t DataCounty_ID;
    uint32_t DataDistrictTown_ID;
    uint16_t Electoral;
    uint16_t StateAssembly;
    uint8_t  StateSenate;
    uint16_t Legislative;
    char Ward[13];
    uint8_t Congress;
} DataDistrict;
enum {
    DISTRICT_FIELD_ZIPCODE = 1,
    DISTRICT_FIELD_DBTABLE = 2,
    DISTRICT_FIELD_DBTABLEVALUE = 3,
    DISTRICT_FIELD_DATACOUNTY_ID = 4,
    DISTRICT_FIELD_DATADISTRICTTOWN_ID = 5,
    DISTRICT_FIELD_ELECTORAL = 6,
    DISTRICT_FIELD_STATEASSEMBLY = 7,
    DISTRICT_FIELD_STATESENATE = 8,
    DISTRICT_FIELD_LEGISLATIVE = 9,
    DISTRICT_FIELD_WARD = 10,
    DISTRICT_FIELD_CONGRESS = 11
};
typedef struct DistrictHashTable DistrictHashTable;
typedef RecordLookupResult DistrictLookupResult;
typedef RecordMatch DistrictMatch;
#define DISTRICT_ERROR RECORD_ERROR
#define DISTRICT_NOT_FOUND RECORD_NOT_FOUND
#define DISTRICT_FOUND RECORD_FOUND
#define DISTRICT_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
DistrictHashTable *BuildDistrictHashTable(const DataDistrict *, size_t);
DistrictMatch FindDistrictID(const DistrictHashTable *, const DataDistrict *);
void FreeDistrictMatch(DistrictMatch *);
void FreeDistrictHashTable(DistrictHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadDistrictIntoMemory(MYSQL *, DataDistrict **, size_t *);

int QueryDistrictMemory(const void *hashTable,const void *record);
int QueryDistrictDatabase(MYSQL *db,const void *record);
static int ParseDistrictQuery(char *line, void *record);

#endif
