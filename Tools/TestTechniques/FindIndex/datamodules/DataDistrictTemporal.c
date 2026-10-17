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

/*
int QueryDistrictTemporalMemory(const void *hashTable, const void *record) {
  DistrictTemporalMatch match = FindDistrictTemporalID(hashTable, record);

  if (match.status == DISTRICT_TEMPORAL_NOT_FOUND) {
    printf("District temporal record not found in memory.\n");
  } else if (match.status == DISTRICT_TEMPORAL_ERROR) {
    fprintf(stderr, "District temporal lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
      printf("District temporal ID: %" PRIu32 "\n",  match.IDs[i]);
    }
  }

  int result = match.status == DISTRICT_TEMPORAL_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;

  FreeDistrictTemporalMatch(&match);
  return result;
}
*/

int QueryDistrictTemporalMemory(const void *hashTable, const void *record) {
  if (hashTable == NULL || record == NULL) {
    fprintf(stderr, "Invalid district-temporal lookup arguments.\n");
    return EXIT_FAILURE;
  }

  const DistrictTemporalHashTable *table = (const DistrictTemporalHashTable *)hashTable;
  const DataDistrictTemporal *temporal = (const DataDistrictTemporal *)record;
  DistrictTemporalMatch match = FindDistrictTemporalID(table, temporal);

  if (match.status == DISTRICT_TEMPORAL_NOT_FOUND) {
  	printf("District-temporal record not found in memory.\n");
  } else if (match.status == DISTRICT_TEMPORAL_ERROR) {
    fprintf(stderr, "District-temporal lookup failed.\n");
  } else {
    if (match.status == DISTRICT_TEMPORAL_DUPLICATE) {
      printf("Duplicate district-temporal records: %zu\n", match.count);
    }

    for (size_t i = 0; i < match.count; ++i) {
      printf("District-temporal ID: %" PRIu32 "\n", match.IDs[i]);
    }
  }

  int result = match.status == DISTRICT_TEMPORAL_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;
  FreeDistrictTemporalMatch(&match);
  return result;
}

int ParseDistrictTemporalQuery(
    char *line,
    void *output)
{
    if (line == NULL || output == NULL) {
        return EXIT_FAILURE;
    }

    DataDistrictTemporal *temporal =
        (DataDistrictTemporal *)output;

    memset(temporal, 0, sizeof(*temporal));

    char *field[3] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 3) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 3 || token != NULL) {
        fprintf(stderr,
                "District-temporal format:\n"
                "DataDistrictCycle_ID,"
                "DataHouse_ID,"
                "DataDistrict_ID\n");
        return EXIT_FAILURE;
    }

    /*
     * Empty fields represent SQL NULL.
     */
    for (size_t i = 0; i < 3; ++i) {
        if (field[i][0] == '\0') {
            temporal->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

    if (!(temporal->nullFields &
          (1u << DISTRICT_TEMPORAL_FIELD_DATADISTRICTCYCLE_ID)) &&
        sscanf(field[0],
               "%" SCNu32,
               &temporal->DataDistrictCycle_ID) != 1) {
        fprintf(stderr,
                "Invalid DataDistrictCycle_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(temporal->nullFields &
          (1u << DISTRICT_TEMPORAL_FIELD_DATAHOUSE_ID)) &&
        sscanf(field[1],
               "%" SCNu32,
               &temporal->DataHouse_ID) != 1) {
        fprintf(stderr,
                "Invalid DataHouse_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(temporal->nullFields &
          (1u << DISTRICT_TEMPORAL_FIELD_DATADISTRICT_ID)) &&
        sscanf(field[2],
               "%" SCNu32,
               &temporal->DataDistrict_ID) != 1) {
        fprintf(stderr,
                "Invalid DataDistrict_ID.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int QueryDistrictTemporalDatabase(MYSQL *db, const void *record) {
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid district-temporal database arguments.\n");
        return EXIT_FAILURE;
    }

    const DataDistrictTemporal *temporal = record;

    const char *sql =
        "SELECT DataDistrictTemporal_ID "
        "FROM DataDistrictTemporal "
        "WHERE DataDistrictCycle_ID = ? "
        "AND DataHouse_ID = ? "
        "AND DataDistrict_ID = ?";

    MYSQL_STMT *stmt = mysql_stmt_init(db);

    if (stmt == NULL) {
        fprintf(stderr, "Statement initialization failed: %s\n",
                mysql_error(db));
        return EXIT_FAILURE;
    }

    if (mysql_stmt_prepare(
            stmt, sql, (unsigned long)strlen(sql)) != 0) {
        fprintf(stderr, "Prepare failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    MYSQL_BIND params[3];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&temporal->DataDistrictCycle_ID;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = (void *)&temporal->DataHouse_ID;

    params[2].buffer_type = MYSQL_TYPE_LONG;
    params[2].buffer = (void *)&temporal->DataDistrict_ID;

    if (mysql_stmt_bind_param(stmt, params) != 0) {
        fprintf(stderr, "Parameter binding failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Execute failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    uint32_t temporalID = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &temporalID;
    result.buffer_length = sizeof(temporalID);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("District-temporal record not found in database.\n");

        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);
        return EXIT_SUCCESS;
    }

    if (fetchStatus != 0 &&
        fetchStatus != MYSQL_DATA_TRUNCATED) {
        fprintf(stderr, "Fetch failed: %s\n",
                mysql_stmt_error(stmt));

        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    printf("District-temporal ID: %" PRIu32 "\n", temporalID);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}