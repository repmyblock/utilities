#include "DataDistrictTemporal.h"
#include <stdlib.h>
struct DistrictTemporalHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(DataDistrictTemporal, DataDistrictCycle_ID), sizeof(((DataDistrictTemporal *)0)->DataDistrictCycle_ID), 0},
    {offsetof(DataDistrictTemporal, DataHouse_ID), sizeof(((DataDistrictTemporal *)0)->DataHouse_ID), 0},
    {offsetof(DataDistrictTemporal, DataDistrict_ID), sizeof(((DataDistrictTemporal *)0)->DataDistrict_ID), 0}
};
DistrictTemporalHashTable *BuildDistrictTemporalHashTable(const DataDistrictTemporal *rows, size_t count)
{
    DistrictTemporalHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(DataDistrictTemporal, ID), offsetof(DataDistrictTemporal, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
DistrictTemporalMatch FindDistrictTemporalID(const DistrictTemporalHashTable *table, const DataDistrictTemporal *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeDistrictTemporalMatch(DistrictTemporalMatch *match) { FreeRecordMatch(match); }
void FreeDistrictTemporalHashTable(DistrictTemporalHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadDistrictTemporalIntoMemory(MYSQL *db, DataDistrictTemporal **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `DataDistrictTemporal_ID`, `DataDistrictCycle_ID`, `DataHouse_ID`, `DataDistrict_ID` FROM `DataDistrictTemporal`",
        sizeof(DataDistrictTemporal), offsetof(DataDistrictTemporal, ID), offsetof(DataDistrictTemporal, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}


int QueryDistrictTemporalMemory(
    const void *hashTable,
    const void *record)
{
    DistrictTemporalMatch match =
        FindDistrictTemporalID(hashTable, record);

    if (match.status == DISTRICTTEMPORAL_NOT_FOUND) {
        printf("District temporal record not found in memory.\n");
    } else if (match.status == DISTRICTTEMPORAL_ERROR) {
        fprintf(stderr, "District temporal lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("District temporal ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == DISTRICTTEMPORAL_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeDistrictTemporalMatch(&match);
    return result;
}