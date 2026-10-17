#include "DataDistrictCycle.h"
#include <stdlib.h>
struct DistrictCycleHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(DataDistrictCycle, CycleStartDate), sizeof(((DataDistrictCycle *)0)->CycleStartDate), 1},
    {offsetof(DataDistrictCycle, CycleEndDate), sizeof(((DataDistrictCycle *)0)->CycleEndDate), 1}
};
DistrictCycleHashTable *BuildDistrictCycleHashTable(const DataDistrictCycle *rows, size_t count)
{
    DistrictCycleHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(DataDistrictCycle, ID), offsetof(DataDistrictCycle, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
DistrictCycleMatch FindDistrictCycleID(const DistrictCycleHashTable *table, const DataDistrictCycle *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeDistrictCycleMatch(DistrictCycleMatch *match) { FreeRecordMatch(match); }
void FreeDistrictCycleHashTable(DistrictCycleHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadDistrictCycleIntoMemory(MYSQL *db, DataDistrictCycle **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `DataDistrictCycle_ID`, `DataDistrictCycle_CycleStartDate`, `DataDistrictCycle_CycleEndDate` FROM `DataDistrictCycle`",
        sizeof(DataDistrictCycle), offsetof(DataDistrictCycle, ID), offsetof(DataDistrictCycle, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}
