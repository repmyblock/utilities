#ifndef DATAPARTYPAC_H
#define DATAPARTYPAC_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataPAC_ID;
    uint32_t DataParty_ID;
    char ValidFrom[16];
    char ValidTo[16];
} DataPartyPAC;

typedef struct PartyPACHashTable PartyPACHashTable;
typedef RecordMatch PartyPACMatch;
typedef RecordLookupResult PartyPACLookupResult;
#define PARTYPAC_ERROR RECORD_ERROR
#define PARTYPAC_NOT_FOUND RECORD_NOT_FOUND
#define PARTYPAC_FOUND RECORD_FOUND
#define PARTYPAC_DUPLICATE RECORD_DUPLICATE

int LoadPartyPACIntoMemory3(MYSQL *, DataPartyPAC **, size_t *);
int LoadPartyPACIntoMemory4(MYSQL *, DataPartyPAC **, size_t *, size_t);
#define PartyPAC_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadPartyPACIntoMemory(...) PartyPAC_LOAD_SELECT(__VA_ARGS__, LoadPartyPACIntoMemory4, LoadPartyPACIntoMemory3)(__VA_ARGS__)
void PrintPartyPAC(const DataPartyPAC *);
int ParsePartyPACQuery(char *, void *);
int ParsePartyPACRecord(char *, void *);
PartyPACHashTable *BuildPartyPACHashTable(const DataPartyPAC *, size_t);
PartyPACMatch FindPartyPACID(const PartyPACHashTable *, const DataPartyPAC *);
void FreePartyPACMatch(PartyPACMatch *);
void FreePartyPACHashTable(PartyPACHashTable *);
int QueryPartyPACMemory(const void *, const void *);
int QueryPartyPACDatabase(MYSQL *, const void *);

#endif
