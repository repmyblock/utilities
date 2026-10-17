#include "VoterIndex.h"
#include <stdlib.h>
struct VoterIndexHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(VoterIndex, DataLastName_ID), sizeof(((VoterIndex *)0)->DataLastName_ID), 0},
    {offsetof(VoterIndex, DataFirstName_ID), sizeof(((VoterIndex *)0)->DataFirstName_ID), 0},
    {offsetof(VoterIndex, DataMiddleName_ID), sizeof(((VoterIndex *)0)->DataMiddleName_ID), 0},
    {offsetof(VoterIndex, Suffix), sizeof(((VoterIndex *)0)->Suffix), 1},
    {offsetof(VoterIndex, DOB), sizeof(((VoterIndex *)0)->DOB), 1},
    {offsetof(VoterIndex, DataState_ID), sizeof(((VoterIndex *)0)->DataState_ID), 0},
    {offsetof(VoterIndex, UniqStateVoterID), sizeof(((VoterIndex *)0)->UniqStateVoterID), 1}
};
VoterIndexHashTable *BuildVoterIndexHashTable(const VoterIndex *rows, size_t count)
{
    VoterIndexHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
VoterIndexMatch FindVoterIndexID(const VoterIndexHashTable *table, const VoterIndex *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeVoterIndexMatch(VoterIndexMatch *match) { FreeRecordMatch(match); }
void FreeVoterIndexHashTable(VoterIndexHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadVoterIndexIntoMemory(MYSQL *db, VoterIndex **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `VotersIndexes_ID`, `DataLastName_ID`, `DataFirstName_ID`, `DataMiddleName_ID`, `VotersIndexes_Suffix`, `VotersIndexes_DOB`, `DataState_ID`, `VotersIndexes_UniqStateVoterID` FROM `VotersIndexes`",
        sizeof(VoterIndex), offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}


int QueryVoterIndexMemory(
    const void *hashTable,
    const void *record)
{
    VoterIndexMatch match =
        FindVoterIndexID(hashTable, record);

    if (match.status == VOTERINDEX_NOT_FOUND) {
        printf("Voter index not found in memory.\n");
    } else if (match.status == VOTERINDEX_ERROR) {
        fprintf(stderr, "Voter-index lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("Voter index ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == VOTERINDEX_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeVoterIndexMatch(&match);
    return result;
}
