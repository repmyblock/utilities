#ifndef VOTERCOMPLEMENTINFO_H
#define VOTERCOMPLEMENTINFO_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t Voters_ID;
    char PrevName[601];
    char PrevAddress[401];
    uint32_t DataCountyID_PrevCounty;
    char LastYearVoted[32];
    char LastDateVoted[16];
    char OtherParty[401];
} VoterComplementInfo;

typedef struct VoterComplementInfoHashTable VoterComplementInfoHashTable;
typedef RecordMatch VoterComplementInfoMatch;
typedef RecordLookupResult VoterComplementInfoLookupResult;
#define VOTERCOMPLEMENTINFO_ERROR RECORD_ERROR
#define VOTERCOMPLEMENTINFO_NOT_FOUND RECORD_NOT_FOUND
#define VOTERCOMPLEMENTINFO_FOUND RECORD_FOUND
#define VOTERCOMPLEMENTINFO_DUPLICATE RECORD_DUPLICATE

int LoadVoterComplementInfoIntoMemory3(MYSQL *, VoterComplementInfo **, size_t *);
int LoadVoterComplementInfoIntoMemory4(MYSQL *, VoterComplementInfo **, size_t *, size_t);
#define VoterComplementInfo_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadVoterComplementInfoIntoMemory(...) VoterComplementInfo_LOAD_SELECT(__VA_ARGS__, LoadVoterComplementInfoIntoMemory4, LoadVoterComplementInfoIntoMemory3)(__VA_ARGS__)
void PrintVoterComplementInfo(const VoterComplementInfo *);
int ParseVoterComplementInfoQuery(char *, void *);
int ParseVoterComplementInfoRecord(char *, void *);
VoterComplementInfoHashTable *BuildVoterComplementInfoHashTable(const VoterComplementInfo *, size_t);
VoterComplementInfoMatch FindVoterComplementInfoID(const VoterComplementInfoHashTable *, const VoterComplementInfo *);
void FreeVoterComplementInfoMatch(VoterComplementInfoMatch *);
void FreeVoterComplementInfoHashTable(VoterComplementInfoHashTable *);
int QueryVoterComplementInfoMemory(const void *, const void *);
int QueryVoterComplementInfoDatabase(MYSQL *, const void *);

#endif
