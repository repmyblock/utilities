/* This is the DataAddress.c to deal with searching this structure */
#ifndef DATA_ADDRESS_H
#define DATA_ADDRESS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t ID;

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
AddressHashTable *BuildAddressHashTable(
    const DataAddress *addresses,
    size_t count
);

/*
 * Match the nine address fields.
 * Ignore ID, Cordinate_ID, and PG_OSM_osmid.
 * Report all matching IDs, including duplicates.
 */
AddressMatch FindAddressID(
    const AddressHashTable *table,
    const DataAddress *search
);

/* Release IDs returned by FindAddressID(). */
void FreeAddressMatch(AddressMatch *match);

/* Release the complete hash table. */
void FreeAddressHashTable(AddressHashTable *table);

#endif