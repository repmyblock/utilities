#include "DataHouse.h"
#include <stdlib.h>
struct HouseHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(DataHouse, DataAddress_ID), sizeof(((DataHouse *)0)->DataAddress_ID), 0},
    {offsetof(DataHouse, Type), sizeof(((DataHouse *)0)->Type), 1},
    {offsetof(DataHouse, Apt), sizeof(((DataHouse *)0)->Apt), 1},
    {offsetof(DataHouse, DataStreetNonStdFormat_ID), sizeof(((DataHouse *)0)->DataStreetNonStdFormat_ID), 0},
    {offsetof(DataHouse, BIN), sizeof(((DataHouse *)0)->BIN), 0}
};
HouseHashTable *BuildHouseHashTable(const DataHouse *rows, size_t count)
{
    HouseHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(DataHouse, ID), offsetof(DataHouse, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
HouseMatch FindHouseID(const HouseHashTable *table, const DataHouse *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeHouseMatch(HouseMatch *match) { FreeRecordMatch(match); }
void FreeHouseHashTable(HouseHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadHouseIntoMemory(MYSQL *db, DataHouse **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `DataHouse_ID`, `DataAddress_ID`, `DataHouse_Type`, `DataHouse_Apt`, `DataStreetNonStdFormat_ID`, `DataHouse_BIN` FROM `DataHouse`",
        sizeof(DataHouse), offsetof(DataHouse, ID), offsetof(DataHouse, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}



/* DataHouse.c */

HouseMatch FindHouseID(
    const HouseHashTable *table,
    const DataHouse *query)
{
    if (table == NULL || query == NULL) {
        HouseMatch result = {
            .status = HOUSE_ERROR,
            .IDs = NULL,
            .count = 0
        };
        return result;
    }

    return FindRecordID(table->records, query);
}

int QueryHouseMemory(
    const void *hashTable,
    const void *record)
{
    HouseMatch match =
        FindHouseID(hashTable, record);

    if (match.status == HOUSE_NOT_FOUND) {
        printf("House not found in memory.\n");
    } else if (match.status == HOUSE_ERROR) {
        fprintf(stderr, "House lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("House ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == HOUSE_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeHouseMatch(&match);
    return result;
}