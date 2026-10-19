#ifndef VOTER_H
#define VOTER_H
#include "RecordHash.h"
#include <mysql/mysql.h>
typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t VotersIndexes_ID;
    uint32_t DataHouse_ID;
    char Gender[13];
    char UniqStateVoterID[201];
    uint32_t DataState_ID;
    char RegParty[13];
    char ReasonCode[20];
    char Status[26];
    uint32_t VotersMailingAddress_ID;
    char IDRequired[4];
    char IDMet[4];
    char ApplyDate[11];
    char RegSource[15];
    char DateInactive[11];
    char DatePurged[11];
    char CountyVoterNumber[201];
    char RMBActive[4];
    char RecFirstSeen[11];
    char RecLastSeen[11];
} Voter;
enum {
    VOTER_FIELD_VOTERSINDEXES_ID = 1,
    VOTER_FIELD_DATAHOUSE_ID = 2,
    VOTER_FIELD_GENDER = 3,
    VOTER_FIELD_UNIQSTATEVOTERID = 4,
    VOTER_FIELD_DATASTATE_ID = 5,
    VOTER_FIELD_REGPARTY = 6,
    VOTER_FIELD_REASONCODE = 7,
    VOTER_FIELD_STATUS = 8,
    VOTER_FIELD_VOTERSMAILINGADDRESS_ID = 9,
    VOTER_FIELD_IDREQUIRED = 10,
    VOTER_FIELD_IDMET = 11,
    VOTER_FIELD_APPLYDATE = 12,
    VOTER_FIELD_REGSOURCE = 13,
    VOTER_FIELD_DATEINACTIVE = 14,
    VOTER_FIELD_DATEPURGED = 15,
    VOTER_FIELD_COUNTYVOTERNUMBER = 16,
    VOTER_FIELD_RMBACTIVE = 17,
    VOTER_FIELD_RECFIRSTSEEN = 18,
    VOTER_FIELD_RECLASTSEEN = 19
};
typedef struct VoterHashTable VoterHashTable;
typedef RecordLookupResult VoterLookupResult;
typedef RecordMatch VoterMatch;
#define VOTER_ERROR RECORD_ERROR
#define VOTER_NOT_FOUND RECORD_NOT_FOUND
#define VOTER_FOUND RECORD_FOUND
#define VOTER_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
VoterHashTable *BuildVoterHashTable(const Voter *, size_t);
VoterMatch FindVoterID(const VoterHashTable *, const Voter *);
void FreeVoterMatch(VoterMatch *);
void FreeVoterHashTable(VoterHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadVoterIntoMemory(MYSQL *, Voter **, size_t *, size_t);


int QueryVoterMemory(
    const void *hashTable,
    const void *record);

int QueryVoterDatabase(
    MYSQL *db,
    const void *record);
#endif
