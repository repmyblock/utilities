#ifndef DATAADDRESS_H
#define DATAADDRESS_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char HouseNumber[401];
    char FracAddress[401];
    char PreStreet[401];
    uint32_t DataStreet_ID;
    char PostStreet[401];
    uint32_t DataCity_ID;
    uint32_t DataCounty_ID;
    char zipcode[121];
    char zip4[41];
    uint32_t Cordinate_ID;
    char PG_OSM_osmid[32];
} DataAddress;

typedef struct AddressHashTable AddressHashTable;
typedef RecordMatch AddressMatch;
typedef RecordLookupResult AddressLookupResult;
#define ADDRESS_ERROR RECORD_ERROR
#define ADDRESS_NOT_FOUND RECORD_NOT_FOUND
#define ADDRESS_FOUND RECORD_FOUND
#define ADDRESS_DUPLICATE RECORD_DUPLICATE

int LoadAddressesIntoMemory3(MYSQL *, DataAddress **, size_t *);
int LoadAddressesIntoMemory4(MYSQL *, DataAddress **, size_t *, size_t);
#define Addresses_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadAddressesIntoMemory(...) Addresses_LOAD_SELECT(__VA_ARGS__, LoadAddressesIntoMemory4, LoadAddressesIntoMemory3)(__VA_ARGS__)
void PrintAddress(const DataAddress *);
int ParseAddressQuery(char *, void *);
int ParseAddressRecord(char *, void *);
AddressHashTable *BuildAddressHashTable(const DataAddress *, size_t);
AddressMatch FindAddressID(const AddressHashTable *, const DataAddress *);
void FreeAddressMatch(AddressMatch *);
void FreeAddressHashTable(AddressHashTable *);
int QueryAddressMemory(const void *, const void *);
int QueryAddressDatabase(MYSQL *, const void *);

#endif
