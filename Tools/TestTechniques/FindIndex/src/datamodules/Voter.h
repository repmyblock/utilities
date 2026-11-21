#ifndef VOTER_H
#define VOTER_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t VotersIndexes_ID;
    uint32_t DataHouse_ID;
    char Gender[49];
    char UniqStateVoterID[201];
    uint32_t DataState_ID;
    char RegParty[13];
    char ReasonCode[77];
    char Status[101];
    uint32_t VotersMailingAddress_ID;
    char IDRequired[13];
    char IDMet[13];
    char ApplyDate[16];
    char RegSource[57];
    char DateInactive[16];
    char DatePurged[16];
    char CountyVoterNumber[201];
    char RMBActive[13];
    char RecFirstSeen[16];
    char RecLastSeen[16];
} Voter;

typedef struct VoterHashTable VoterHashTable;
typedef RecordMatch VoterMatch;
typedef RecordLookupResult VoterLookupResult;
#define VOTER_ERROR RECORD_ERROR
#define VOTER_NOT_FOUND RECORD_NOT_FOUND
#define VOTER_FOUND RECORD_FOUND
#define VOTER_DUPLICATE RECORD_DUPLICATE

int LoadVoterIntoMemory3(MYSQL *, Voter **, size_t *);
int LoadVoterIntoMemory4(MYSQL *, Voter **, size_t *, size_t);
#define Voter_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadVoterIntoMemory(...) Voter_LOAD_SELECT(__VA_ARGS__, LoadVoterIntoMemory4, LoadVoterIntoMemory3)(__VA_ARGS__)
void PrintVoter(const Voter *);
int ParseVoterQuery(char *, void *);
int ParseVoterRecord(char *, void *);
VoterHashTable *BuildVoterHashTable(const Voter *, size_t);
VoterMatch FindVoterID(const VoterHashTable *, const Voter *);
void FreeVoterMatch(VoterMatch *);
void FreeVoterHashTable(VoterHashTable *);
int QueryVoterMemory(const void *, const void *);
int QueryVoterDatabase(MYSQL *, const void *);

#endif
