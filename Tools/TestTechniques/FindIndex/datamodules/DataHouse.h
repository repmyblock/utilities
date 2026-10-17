#ifndef DATA_HOUSE_H
#define DATA_HOUSE_H
#include "RecordHash.h"
#include <mysql/mysql.h>
typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t DataAddress_ID;
    char Type[41];
    char Apt[401];
    uint32_t DataStreetNonStdFormat_ID;
    uint32_t BIN;
} DataHouse;
enum {
    HOUSE_FIELD_DATAADDRESS_ID = 1,
    HOUSE_FIELD_TYPE = 2,
    HOUSE_FIELD_APT = 3,
    HOUSE_FIELD_DATASTREETNONSTDFORMAT_ID = 4,
    HOUSE_FIELD_BIN = 5
};
typedef struct HouseHashTable HouseHashTable;
typedef RecordLookupResult HouseLookupResult;
typedef RecordMatch HouseMatch;
#define HOUSE_ERROR RECORD_ERROR
#define HOUSE_NOT_FOUND RECORD_NOT_FOUND
#define HOUSE_FOUND RECORD_FOUND
#define HOUSE_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
HouseHashTable *BuildHouseHashTable(const DataHouse *, size_t);
HouseMatch FindHouseID(const HouseHashTable *, const DataHouse *);
void FreeHouseMatch(HouseMatch *);
void FreeHouseHashTable(HouseHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadHouseIntoMemory(MYSQL *, DataHouse **, size_t *);


int QueryHouseMemory(const void *hashTable,const void *record);
int QueryHouseDatabase(MYSQL *db,const void *record);
#endif


