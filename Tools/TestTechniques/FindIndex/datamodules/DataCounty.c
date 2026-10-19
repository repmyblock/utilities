#include "DataCounty.h"
#include <stdlib.h>
struct CountyHashTable { RecordHashTable *records; };

static const RecordField fields[] = {
    {offsetof(DataCounty, DataState_ID), sizeof(((DataCounty *)0)->DataState_ID), 0},
    {offsetof(DataCounty, Name), sizeof(((DataCounty *)0)->Name), 1},
    {offsetof(DataCounty, BOEID), sizeof(((DataCounty *)0)->BOEID), 0},
    {offsetof(DataCounty, GEOID), sizeof(((DataCounty *)0)->GEOID), 0}
};

CountyHashTable *BuildCountyHashTable(const DataCounty *rows, size_t count)
{
    CountyHashTable *table = malloc(sizeof(*table));
    if (!table) return NULL;
    table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
        offsetof(DataCounty, ID), offsetof(DataCounty, nullFields), fields,
        sizeof(fields)/sizeof(fields[0]));
    if (!table->records) { free(table); return NULL; }
    return table;
}

void FreeCountyMatch(CountyMatch *match) { FreeRecordMatch(match); }
void FreeCountyHashTable(CountyHashTable *table)
{ if (table) { FreeRecordHashTable(table->records); free(table); } }

#include "RecordLoader.h"
int LoadCountyIntoMemory(MYSQL *db, DataCounty **rows, size_t *count)
{
    if (!rows || !count) return EXIT_FAILURE;
    *rows = NULL; *count = 0;
    void *data = NULL;
    int status = LoadRecordArray(db,
        "SELECT `DataCounty_ID`, `DataState_ID`, `DataCounty_Name`, `DataCounty_BOEID`, `DataCounty_GEOID` FROM `DataCounty`",
        sizeof(DataCounty), offsetof(DataCounty, ID), offsetof(DataCounty, nullFields),
        fields, sizeof(fields)/sizeof(fields[0]), &data, count);
    *rows = data;
    return status;
}


/* DataCounty.c */
/*
CountyMatch FindCountyID(const CountyHashTable *table, const DataCounty *query){
	 return FindRecordID(table ? table->records : NULL, query); 
} */


CountyMatch FindCountyID(const CountyHashTable *table, const DataCounty *query) {
  if (table == NULL || query == NULL) {
    CountyMatch result = {
    	.status = COUNTY_ERROR, .IDs = NULL, .count = 0
    };
    return result;
  }
  return FindRecordID(table->records, query);
}

int QueryCountyDatabase(MYSQL *db, const void *record) {
	const DataCounty *county = record;

	if (db == NULL || county == NULL) {
	  return EXIT_FAILURE;
	}

	const char *sql =
	  "SELECT DataCounty_ID FROM DataCounty WHERE DataState_ID = ? "
	  "AND DataCounty_Name = ? AND DataCounty_BOEID = ? AND DataCounty_GEOID = ?";
	MYSQL_STMT *stmt = mysql_stmt_init(db);

	if (stmt == NULL ||
	  mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
	  fprintf(stderr, "County query preparation failed: %s\n", stmt ? mysql_stmt_error(stmt) : mysql_error(db));

	  if (stmt != NULL) {
      mysql_stmt_close(stmt);
    }
    return EXIT_FAILURE;
  }

  unsigned long nameLength = (unsigned long)strlen(county->Name);

  MYSQL_BIND input[4] = {0};

  input[0].buffer_type = MYSQL_TYPE_LONG;
  input[0].buffer = (void *)&county->DataState_ID;

  input[1].buffer_type = MYSQL_TYPE_STRING;
  // input[1].buffer = county->Name;
  input[1].buffer_length = sizeof(county->Name);
  input[1].length = &nameLength;

  input[2].buffer_type = MYSQL_TYPE_LONG;
  input[2].buffer = (void *)&county->BOEID;

  input[3].buffer_type = MYSQL_TYPE_LONG;
  input[3].buffer = (void *)&county->GEOID;

  if (mysql_stmt_bind_param(stmt, input) != 0 ||
    mysql_stmt_execute(stmt) != 0) {
    fprintf(stderr, "County query failed: %s\n", mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    return EXIT_FAILURE;
  }

  uint32_t id = 0;
  MYSQL_BIND output = {0};

  output.buffer_type = MYSQL_TYPE_LONG;
  output.buffer = &id;
  output.buffer_length = sizeof(id);

  if (mysql_stmt_bind_result(stmt, &output) != 0 || mysql_stmt_store_result(stmt) != 0) {
    fprintf(stderr, "County result failed: %s\n", mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    return EXIT_FAILURE;
  }

  int found = 0;
  int fetchStatus;

  while ((fetchStatus = mysql_stmt_fetch(stmt)) == 0) {
    printf("Database County ID: %" PRIu32 "\n", id);
    found = 1;
  }

  if (fetchStatus != MYSQL_NO_DATA) {
    fprintf(stderr, "County fetch failed: %s\n", mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    return EXIT_FAILURE;
  }

  if (!found) {
    printf("County not found in database.\n");
  }

  mysql_stmt_close(stmt);
  return EXIT_SUCCESS;
}

int QueryCountyMemory(const void *hashTable, const void *record) {
  if (hashTable == NULL || record == NULL) {
    fprintf(stderr, "Invalid county lookup arguments.\n");
    return EXIT_FAILURE;
  }

  const CountyHashTable *table = (const CountyHashTable *)hashTable;
  const DataCounty *county = (const DataCounty *)record;
  CountyMatch match = FindCountyID(table, county);

  if (match.status == COUNTY_NOT_FOUND) {
    printf("County not found in memory.\n");
  } else if (match.status == COUNTY_ERROR) {
    fprintf(stderr, "County lookup failed.\n");
  } else {
    if (match.status == COUNTY_DUPLICATE) {
      printf("Duplicate county records: %zu\n", match.count);
    }

    for (size_t i = 0; i < match.count; ++i) {
      printf("County ID: %" PRIu32 "\n", match.IDs[i]);
    }
  }

  int result = match.status == COUNTY_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;
  FreeCountyMatch(&match);
  return result;
}

/*
int QueryCountyMemory(const void *hashTable, const void *record) {
  if (hashTable == NULL || record == NULL) {
    return EXIT_FAILURE;
  }

  CountyMatch match = FindCountyID(hashTable, record);
  if (match.status == COUNTY_NOT_FOUND) {
    printf("County not found in memory.\n");
    FreeCountyMatch(&match);
    return EXIT_SUCCESS;
  }

  if (match.status == COUNTY_ERROR) {
    fprintf(stderr, "County memory lookup failed.\n");
    FreeCountyMatch(&match);
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < match.count; ++i) {
    printf("County ID: %" PRIu32 "\n", match.IDs[i]);
  }

  FreeCountyMatch(&match);
  return EXIT_SUCCESS;
}
*/

int ParseCountyQuery(char *line, void *output) {
    if (line == NULL || output == NULL) {
        return EXIT_FAILURE;
    }

    DataCounty *county =
        (DataCounty *)output;

    memset(county, 0, sizeof(*county));

    char *field[4] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 4) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 4 || token != NULL) {
        fprintf(stderr,
                "County format:\n"
                "DataState_ID,DataCounty_Name,"
                "DataCounty_BOEID,DataCounty_GEOID\n");
        return EXIT_FAILURE;
    }

    /*
     * Empty fields represent SQL NULL.
     */
    for (size_t i = 0; i < 4; ++i) {
        if (field[i][0] == '\0') {
            county->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

    if (!(county->nullFields &
          (1u << COUNTY_FIELD_DATASTATE_ID))) {
        if (sscanf(field[0],
                   "%" SCNu32,
                   &county->DataState_ID) != 1) {
            fprintf(stderr,
                    "Invalid DataState_ID.\n");
            return EXIT_FAILURE;
        }
    }

    if (!(county->nullFields &
          (1u << COUNTY_FIELD_NAME))) {
        snprintf(
            county->Name,
            sizeof(county->Name),
            "%s",
            field[1]);
    }

    if (!(county->nullFields &
          (1u << COUNTY_FIELD_BOEID))) {
        if (sscanf(field[2],
                   "%" SCNu32,
                   &county->BOEID) != 1) {
            fprintf(stderr,
                    "Invalid DataCounty_BOEID.\n");
            return EXIT_FAILURE;
        }
    }

    if (!(county->nullFields &
          (1u << COUNTY_FIELD_GEOID))) {
        if (sscanf(field[3],
                   "%" SCNu32,
                   &county->GEOID) != 1) {
            fprintf(stderr,
                    "Invalid DataCounty_GEOID.\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}