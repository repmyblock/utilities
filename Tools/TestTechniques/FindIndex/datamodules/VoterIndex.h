#ifndef VOTER_INDEX_H
#define VOTER_INDEX_H
#include "RecordHash.h"
#include <mysql/mysql.h>
typedef struct {
    uint32_t ID;
    /* Bit N: SQL NULL in field N below, counting ID as column zero. */
    uint32_t nullFields;
    uint32_t DataLastName_ID;
    uint32_t DataFirstName_ID;
    uint32_t DataMiddleName_ID;
    char Suffix[41];
    char DOB[11];
    uint32_t DataState_ID;
    char UniqStateVoterID[81];
} VoterIndex;
enum {
    VOTER_INDEX_FIELD_DATALASTNAME_ID = 1,
    VOTER_INDEX_FIELD_DATAFIRSTNAME_ID = 2,
    VOTER_INDEX_FIELD_DATAMIDDLENAME_ID = 3,
    VOTER_INDEX_FIELD_SUFFIX = 4,
    VOTER_INDEX_FIELD_DOB = 5,
    VOTER_INDEX_FIELD_DATASTATE_ID = 6,
    VOTER_INDEX_FIELD_UNIQSTATEVOTERID = 7
};
typedef struct VoterIndexHashTable VoterIndexHashTable;
typedef RecordLookupResult VoterIndexLookupResult;
typedef RecordMatch VoterIndexMatch;
#define VOTER_INDEX_ERROR RECORD_ERROR
#define VOTER_INDEX_NOT_FOUND RECORD_NOT_FOUND
#define VOTER_INDEX_FOUND RECORD_FOUND
#define VOTER_INDEX_DUPLICATE RECORD_DUPLICATE
/* All non-ID fields form the key. NULL differs from empty/zero.
 * Text is UTF-8, trimmed at edges, canonically normalized and case-folded.
 * Input is copied into owned normalized keys; source array may be freed.
 * Build: NULL on error. Find: ERROR on invalid input/allocation failure.
 * Free every match and table with the corresponding functions below.
 */
VoterIndexHashTable *BuildVoterIndexHashTable(const VoterIndex *, size_t);
VoterIndexMatch FindVoterIndexID(const VoterIndexHashTable *, const VoterIndex *);
void FreeVoterIndexMatch(VoterIndexMatch *);
void FreeVoterIndexHashTable(VoterIndexHashTable *);
/* Caller owns the array; NULL fields are recorded in nullFields. */
int LoadVoterIndexIntoMemory(MYSQL *, VoterIndex **, size_t *);

int QueryVoterIndexMemory(
    const void *hashTable,
    const void *record);

int QueryVoterIndexDatabase(
    MYSQL *db,
    const void *record);
#endif
