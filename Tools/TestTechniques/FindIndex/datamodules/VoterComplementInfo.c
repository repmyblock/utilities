#include "VoterComplementInfo.h"
#include <stdlib.h>
struct VoterComplementInfoHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(VoterComplementInfo, Voters_ID), sizeof(((VoterComplementInfo *)0)->Voters_ID), 0},
    {offsetof(VoterComplementInfo, PrevName), sizeof(((VoterComplementInfo *)0)->PrevName), 1},
    {offsetof(VoterComplementInfo, PrevAddress), sizeof(((VoterComplementInfo *)0)->PrevAddress), 1},
    {offsetof(VoterComplementInfo, DataCountyID_PrevCounty), sizeof(((VoterComplementInfo *)0)->DataCountyID_PrevCounty), 0},
    {offsetof(VoterComplementInfo, LastYearVoted), sizeof(((VoterComplementInfo *)0)->LastYearVoted), 0},
    {offsetof(VoterComplementInfo, LastDateVoted), sizeof(((VoterComplementInfo *)0)->LastDateVoted), 1},
    {offsetof(VoterComplementInfo, OtherParty), sizeof(((VoterComplementInfo *)0)->OtherParty), 1}
};
VoterComplementInfoHashTable *BuildVoterComplementInfoHashTable(const VoterComplementInfo *rows, size_t count)
{
    VoterComplementInfoHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
VoterComplementInfoMatch FindVoterComplementInfoID(const VoterComplementInfoHashTable *table, const VoterComplementInfo *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeVoterComplementInfoMatch(VoterComplementInfoMatch *match) { FreeRecordMatch(match); }
void FreeVoterComplementInfoHashTable(VoterComplementInfoHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadVoterComplementInfoIntoMemory(MYSQL *db, VoterComplementInfo **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `VotersComplementInfo_ID`, `Voters_ID`, `VotersComplementInfo_PrevName`, `VotersComplementInfo_PrevAddress`, `DataCountyID_PrevCounty`, `VotersComplementInfo_LastYearVoted`, `VotersComplementInfo_LastDateVoted`, `VotersComplementInfo_OtherParty` FROM `VotersComplementInfo`",
        sizeof(VoterComplementInfo), offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}



int QueryVoterComplementInfoMemory(
    const void *hashTable,
    const void *record)
{
    VoterComplementInfoMatch match =
        FindVoterComplementInfoID(hashTable, record);

    if (match.status == VOTERCOMPLEMENTINFO_NOT_FOUND) {
        printf("Voter complement information not found in memory.\n");
    } else if (match.status == VOTERCOMPLEMENTINFO_ERROR) {
        fprintf(stderr,
                "Voter complement-information lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("Voter complement-information ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == VOTERCOMPLEMENTINFO_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeVoterComplementInfoMatch(&match);
    return result;
}