#ifndef VOTER_COMPLEMENT_INFO_H
#define VOTER_COMPLEMENT_INFO_H
#include "RecordHash.h"
#include <mysql/mysql.h>
typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t Voters_ID;
    char PrevName[601];
    char PrevAddress[401];
    uint32_t DataCountyID_PrevCounty;
    uint16_t LastYearVoted;
    char LastDateVoted[11];
    char OtherParty[401];
} VoterComplementInfo;
enum {
    VOTER_COMPLEMENT_INFO_FIELD_VOTERS_ID = 1,
    VOTER_COMPLEMENT_INFO_FIELD_PREVNAME = 2,
    VOTER_COMPLEMENT_INFO_FIELD_PREVADDRESS = 3,
    VOTER_COMPLEMENT_INFO_FIELD_DATACOUNTYID_PREVCOUNTY = 4,
    VOTER_COMPLEMENT_INFO_FIELD_LASTYEARVOTED = 5,
    VOTER_COMPLEMENT_INFO_FIELD_LASTDATEVOTED = 6,
    VOTER_COMPLEMENT_INFO_FIELD_OTHERPARTY = 7
};
typedef struct VoterComplementInfoHashTable VoterComplementInfoHashTable;
typedef RecordLookupResult VoterComplementInfoLookupResult;
typedef RecordMatch VoterComplementInfoMatch;
#define VOTER_COMPLEMENT_INFO_ERROR RECORD_ERROR
#define VOTER_COMPLEMENT_INFO_NOT_FOUND RECORD_NOT_FOUND
#define VOTER_COMPLEMENT_INFO_FOUND RECORD_FOUND
#define VOTER_COMPLEMENT_INFO_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
VoterComplementInfoHashTable *BuildVoterComplementInfoHashTable(const VoterComplementInfo *, size_t);
VoterComplementInfoMatch FindVoterComplementInfoID(const VoterComplementInfoHashTable *, const VoterComplementInfo *);
void FreeVoterComplementInfoMatch(VoterComplementInfoMatch *);
void FreeVoterComplementInfoHashTable(VoterComplementInfoHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadVoterComplementInfoIntoMemory(MYSQL *, VoterComplementInfo **, size_t *);

int QueryVoterComplementInfoMemory(
    const void *hashTable,
    const void *record);

int QueryVoterComplementInfoDatabase(
    MYSQL *db,
    const void *record);
#endif
