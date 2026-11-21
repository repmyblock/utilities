#ifndef DATAPARTY_H
#define DATAPARTY_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataState_ID;
    char Abbrev[13];
    char Name[1021];
    char Recognized[13];
} DataParty;

typedef struct PartyHashTable PartyHashTable;
typedef RecordMatch PartyMatch;
typedef RecordLookupResult PartyLookupResult;
#define PARTY_ERROR RECORD_ERROR
#define PARTY_NOT_FOUND RECORD_NOT_FOUND
#define PARTY_FOUND RECORD_FOUND
#define PARTY_DUPLICATE RECORD_DUPLICATE

int LoadPartyIntoMemory3(MYSQL *, DataParty **, size_t *);
int LoadPartyIntoMemory4(MYSQL *, DataParty **, size_t *, size_t);
#define Party_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadPartyIntoMemory(...) Party_LOAD_SELECT(__VA_ARGS__, LoadPartyIntoMemory4, LoadPartyIntoMemory3)(__VA_ARGS__)
void PrintParty(const DataParty *);
int ParsePartyQuery(char *, void *);
int ParsePartyRecord(char *, void *);
PartyHashTable *BuildPartyHashTable(const DataParty *, size_t);
PartyMatch FindPartyID(const PartyHashTable *, const DataParty *);
void FreePartyMatch(PartyMatch *);
void FreePartyHashTable(PartyHashTable *);
int QueryPartyMemory(const void *, const void *);
int QueryPartyDatabase(MYSQL *, const void *);

#endif
