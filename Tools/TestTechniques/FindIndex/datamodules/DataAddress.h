/* This is the DataAddress.c to deal with searching this structure */
#ifndef DATA_ADDRESS_H
#define DATA_ADDRESS_H
#include "RecordHash.h"

typedef struct {
    uint32_t ID;
    /* Bit N corresponds to SELECT column N below; set means SQL NULL. */
    uint16_t nullFields;

    char HouseNumber[401];
    char FracAddress[401];
    char PreStreet[401];

    uint32_t DataStreet_ID;

    char PostStreet[401];

    uint32_t DataCity_ID;
    uint32_t DataCounty_ID;

    char zipcode[121];
    char zip4[41];

    /* Not included in matching. */
    uint32_t Cordinate_ID;
    int64_t PG_OSM_osmid;
} DataAddress;

/* Column positions used by nullFields (ID is never NULL). */
typedef enum {
    ADDRESS_HOUSE_NUMBER = 1, ADDRESS_FRAC_ADDRESS, ADDRESS_PRE_STREET,
    ADDRESS_STREET_ID, ADDRESS_POST_STREET, ADDRESS_CITY_ID,
    ADDRESS_COUNTY_ID, ADDRESS_ZIPCODE, ADDRESS_ZIP4,
    ADDRESS_COORDINATE_ID, ADDRESS_OSM_ID
} AddressColumn;

/* Returns EXIT_SUCCESS/EXIT_FAILURE. On failure outputs are NULL/0.
 * Caller owns the returned array and must free it. Connection stays open.
 * Values are UTF-8; NULL fields have zero/empty storage and a nullFields bit.
 */
int LoadAddressesIntoMemory(MYSQL *db, DataAddress **addresses, size_t *count);

/* Hide hash table internals from other source files. */
typedef struct AddressHashTable AddressHashTable;

typedef enum {
    ADDRESS_ERROR = -1,
    ADDRESS_NOT_FOUND = 0,
    ADDRESS_FOUND = 1,
    ADDRESS_DUPLICATE = 2
} AddressLookupResult;

typedef struct {
    AddressLookupResult status;
    uint32_t *IDs;
    size_t count;
} AddressMatch;

/*
 * Build an independently owned index of normalized keys and IDs.
 * Return NULL on failure.
 */
AddressHashTable *BuildAddressHashTable(const DataAddress *, size_t);

/*
 * Match the nine address fields.
 * Ignore ID, Cordinate_ID, and PG_OSM_osmid.
 * Report all matching IDs, including duplicates.
 */
AddressMatch FindAddressID(const AddressHashTable *, const DataAddress *);

/* Release IDs returned by FindAddressID(). */
void FreeAddressMatch(AddressMatch *);

/* Release the complete hash table. */
void FreeAddressHashTable(AddressHashTable *);
int FindAddressInDB(MYSQL *, const DataAddress *);
int QueryAddressMemory(const void *hashTable, const void *record);
int QueryAddressDatabase(MYSQL *db,const void *record);
int ParseAddressQuery(char *,DataAddress *);
int ParseAddressRecord(char *,void *);
int ParseAddressRecord(char *line, void *record);

#endif


