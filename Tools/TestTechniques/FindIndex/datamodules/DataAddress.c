#include "DataAddress.h"

#include <stdlib.h>
#include <string.h>

/* Private: normalized UTF-8 strings owned by this key. */
typedef struct {
    char *HouseNumber;
    char *FracAddress;
    char *PreStreet;
    char *PostStreet;
    char *zipcode;
    char *zip4;

    uint32_t DataStreet_ID;
    uint32_t DataCity_ID;
    uint32_t DataCounty_ID;
} AddressKey;

typedef struct {
    AddressKey key;
    uint64_t hash;
    uint32_t ID;
    size_t next;
} AddressHashEntry;

/* Definition of the opaque type declared in DataAddress.h. */
struct AddressHashTable {
    size_t *buckets;
    AddressHashEntry *entries;
    size_t bucketCount;
    size_t count;
};

/*
 * Implement these public functions here:
 *
 * BuildAddressHashTable()
 * FindAddressID()
 * FreeAddressHashTable()
 *
 * Unicode normalization, hashing, and key-comparison helpers
 * should also live here and be declared static.
 */

void FreeAddressMatch(AddressMatch *match)
{
    if (match == NULL) {
        return;
    }

    free(match->IDs);

    *match = (AddressMatch){
        .status = ADDRESS_NOT_FOUND,
        .IDs = NULL,
        .count = 0
    };
}