#include "Voter.h"
#include <stdlib.h>
struct VoterHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(Voter, VotersIndexes_ID), sizeof(((Voter *)0)->VotersIndexes_ID), 0},
    {offsetof(Voter, DataHouse_ID), sizeof(((Voter *)0)->DataHouse_ID), 0},
    {offsetof(Voter, Gender), sizeof(((Voter *)0)->Gender), 1},
    {offsetof(Voter, UniqStateVoterID), sizeof(((Voter *)0)->UniqStateVoterID), 1},
    {offsetof(Voter, DataState_ID), sizeof(((Voter *)0)->DataState_ID), 0},
    {offsetof(Voter, RegParty), sizeof(((Voter *)0)->RegParty), 1},
    {offsetof(Voter, ReasonCode), sizeof(((Voter *)0)->ReasonCode), 1},
    {offsetof(Voter, Status), sizeof(((Voter *)0)->Status), 1},
    {offsetof(Voter, VotersMailingAddress_ID), sizeof(((Voter *)0)->VotersMailingAddress_ID), 0},
    {offsetof(Voter, IDRequired), sizeof(((Voter *)0)->IDRequired), 1},
    {offsetof(Voter, IDMet), sizeof(((Voter *)0)->IDMet), 1},
    {offsetof(Voter, ApplyDate), sizeof(((Voter *)0)->ApplyDate), 1},
    {offsetof(Voter, RegSource), sizeof(((Voter *)0)->RegSource), 1},
    {offsetof(Voter, DateInactive), sizeof(((Voter *)0)->DateInactive), 1},
    {offsetof(Voter, DatePurged), sizeof(((Voter *)0)->DatePurged), 1},
    {offsetof(Voter, CountyVoterNumber), sizeof(((Voter *)0)->CountyVoterNumber), 1},
    {offsetof(Voter, RMBActive), sizeof(((Voter *)0)->RMBActive), 1},
    {offsetof(Voter, RecFirstSeen), sizeof(((Voter *)0)->RecFirstSeen), 1},
    {offsetof(Voter, RecLastSeen), sizeof(((Voter *)0)->RecLastSeen), 1}
};
VoterHashTable *BuildVoterHashTable(const Voter *rows, size_t count)
{
    VoterHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(Voter, ID), offsetof(Voter, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
VoterMatch FindVoterID(const VoterHashTable *table, const Voter *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeVoterMatch(VoterMatch *match) { FreeRecordMatch(match); }
void FreeVoterHashTable(VoterHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadVoterIntoMemory(MYSQL *db, Voter **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `Voters_ID`, `VotersIndexes_ID`, `DataHouse_ID`, `Voters_Gender`, `Voters_UniqStateVoterID`, `DataState_ID`, `Voters_RegParty`, `Voters_ReasonCode`, `Voters_Status`, `VotersMailingAddress_ID`, `Voters_IDRequired`, `Voters_IDMet`, `Voters_ApplyDate`, `Voters_RegSource`, `Voters_DateInactive`, `Voters_DatePurged`, `Voters_CountyVoterNumber`, `Voters_RMBActive`, `Voters_RecFirstSeen`, `Voters_RecLastSeen` FROM `Voters`",
        sizeof(Voter), offsetof(Voter, ID), offsetof(Voter, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}



/* Voter.c */

VoterMatch FindVoterID(
    const VoterHashTable *table,
    const Voter *query)
{
    if (table == NULL || query == NULL) {
        VoterMatch result = {
            .status = VOTER_ERROR,
            .IDs = NULL,
            .count = 0
        };
        return result;
    }

    return FindRecordID(table->records, query);
}

int QueryVoterMemory(
    const void *hashTable,
    const void *record)
{
    VoterMatch match =
        FindVoterID(hashTable, record);

    if (match.status == VOTER_NOT_FOUND) {
        printf("Voter not found in memory.\n");
    } else if (match.status == VOTER_ERROR) {
        fprintf(stderr, "Voter lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("Voter ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == VOTER_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeVoterMatch(&match);
    return result;
}