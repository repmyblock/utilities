#include "DataDistrict.h"
#include <stdlib.h>
struct DistrictHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(DataDistrict, ZipCode), sizeof(((DataDistrict *)0)->ZipCode), 1},
    {offsetof(DataDistrict, DBTable), sizeof(((DataDistrict *)0)->DBTable), 1},
    {offsetof(DataDistrict, DBTableValue), sizeof(((DataDistrict *)0)->DBTableValue), 1},
    {offsetof(DataDistrict, DataCounty_ID), sizeof(((DataDistrict *)0)->DataCounty_ID), 0},
    {offsetof(DataDistrict, DataDistrictTown_ID), sizeof(((DataDistrict *)0)->DataDistrictTown_ID), 0},
    {offsetof(DataDistrict, Electoral), sizeof(((DataDistrict *)0)->Electoral), 0},
    {offsetof(DataDistrict, StateAssembly), sizeof(((DataDistrict *)0)->StateAssembly), 0},
    {offsetof(DataDistrict, StateSenate), sizeof(((DataDistrict *)0)->StateSenate), 0},
    {offsetof(DataDistrict, Legislative), sizeof(((DataDistrict *)0)->Legislative), 0},
    {offsetof(DataDistrict, Ward), sizeof(((DataDistrict *)0)->Ward), 1},
    {offsetof(DataDistrict, Congress), sizeof(((DataDistrict *)0)->Congress), 0}
};
DistrictHashTable *BuildDistrictHashTable(const DataDistrict *rows, size_t count)
{
    DistrictHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}
DistrictMatch FindDistrictID(const DistrictHashTable *table, const DataDistrict *query)
{ return FindRecordID(table ? table->records : NULL, query); }
void FreeDistrictMatch(DistrictMatch *match) { FreeRecordMatch(match); }
void FreeDistrictHashTable(DistrictHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadDistrictIntoMemory(MYSQL *db, DataDistrict **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `DataDistrict_ID`, `DataDistrict_ZipCode`, `DataDistrict_DBTable`, `DataDistrict_DBTableValue`, `DataCounty_ID`, `DataDistrictTown_ID`, `DataDistrict_Electoral`, `DataDistrict_StateAssembly`, `DataDistrict_StateSenate`, `DataDistrict_Legislative`, `DataDistrict_Ward`, `DataDistrict_Congress` FROM `DataDistrict`",
        sizeof(DataDistrict), offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}



/* DataDistrict.c */

DistrictMatch FindDistrictID(
    const DistrictHashTable *table,
    const DataDistrict *query)
{
    if (table == NULL || query == NULL) {
        DistrictMatch result = {
            .status = DISTRICT_ERROR,
            .IDs = NULL,
            .count = 0
        };
        return result;
    }

    return FindRecordID(table->records, query);
}

int QueryDistrictMemory(
    const void *hashTable,
    const void *record)
{
    DistrictMatch match =
        FindDistrictID(hashTable, record);

    if (match.status == DISTRICT_NOT_FOUND) {
        printf("District not found in memory.\n");
    } else if (match.status == DISTRICT_ERROR) {
        fprintf(stderr, "District lookup failed.\n");
    } else {
        for (size_t i = 0; i < match.count; ++i) {
            printf("District ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == DISTRICT_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeDistrictMatch(&match);
    return result;
}