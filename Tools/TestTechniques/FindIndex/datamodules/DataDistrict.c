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

DistrictHashTable *BuildDistrictHashTable(const DataDistrict *rows, size_t count) {
  DistrictHashTable *table = malloc(sizeof(*table));
  if (!table) return NULL;
  table->records = BuildRecordHashTable(
		rows, count, sizeof(*rows), offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields), 
		fields, sizeof(fields)/sizeof(fields[0])
	);
  if (!table->records) { free(table); return NULL; }
  return table;
}

void FreeDistrictMatch(DistrictMatch *match) { FreeRecordMatch(match); }
void FreeDistrictHashTable(DistrictHashTable *table) { 
	if (table) { FreeRecordHashTable(table->records); free(table); } 
}

#include "RecordLoader.h"
int LoadDistrictIntoMemory(MYSQL *db, DataDistrict **rows, size_t *count) {
  if (!rows || !count) return EXIT_FAILURE;
  *rows = NULL; *count = 0;
  void *data = NULL;
  int status = LoadRecordArray(db,
      "SELECT `DataDistrict_ID`, `DataDistrict_ZipCode`, `DataDistrict_DBTable`, `DataDistrict_DBTableValue`, " 
      "`DataCounty_ID`, `DataDistrictTown_ID`, `DataDistrict_Electoral`, `DataDistrict_StateAssembly`, "
      "`DataDistrict_StateSenate`, `DataDistrict_Legislative`, `DataDistrict_Ward`, `DataDistrict_Congress` " 
      "FROM `DataDistrict`",
      sizeof(DataDistrict), offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields),
      fields, sizeof(fields)/sizeof(fields[0]), &data, count);
  *rows = data;
  return status;
}

/* DataDistrict.c */

/*
DistrictMatch FindDistrictID(const DistrictHashTable *table, const DataDistrict *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
}
*/

DistrictMatch FindDistrictID(const DistrictHashTable *table, const DataDistrict *query) {
	if (table == NULL || query == NULL) {
		DistrictMatch result = {
		  .status = DISTRICT_ERROR, .IDs = NULL, .count = 0
		};
		return result;
	}

	return FindRecordID(table->records, query);
}

int QueryDistrictMemory(const void *hashTable, const void *record) {
  DistrictMatch match = FindDistrictID(hashTable, record);

  if (match.status == DISTRICT_NOT_FOUND) {
    printf("District not found in memory.\n");
  } else if (match.status == DISTRICT_ERROR) {
    fprintf(stderr, "District lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
      printf("District ID: %" PRIu32 "\n", match.IDs[i]);
    }
  }

  int result = match.status == DISTRICT_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;
  FreeDistrictMatch(&match);
  return result;
}

int ParseDistrictQuery(char *line, void *output) {
  if (line == NULL || output == NULL) {
  	return EXIT_FAILURE;
  }

  DataDistrict *district = (DataDistrict *)output;
  memset(district, 0, sizeof(*district));
  char *field[11] = {0};
  size_t count = 0;

  char *token = strtok(line, ",");

  while (token != NULL && count < 11) {
    field[count++] = token;
    token = strtok(NULL, ",");
  }

  if (count != 11 || token != NULL) {
    fprintf(stderr, "District format:\n"
        "ZipCode,DBTable,DBTableValue,DataCounty_ID,DataDistrictTown_ID,"
        "Electoral,StateAssembly,StateSenate,Legislative,Ward,Congress\n");
    return EXIT_FAILURE;
  }

  /*
   * Empty fields represent SQL NULL.
   * The bit number corresponds to the field number.
   */
  for (size_t i = 0; i < 11; ++i) {
    if (field[i][0] == '\0') {
      district->nullFields |= (uint32_t)1u << (i + 1);
    }
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_ZIPCODE))) {
    snprintf(district->ZipCode, sizeof(district->ZipCode), "%s", field[0]);
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_DBTABLE))) {
    snprintf(district->DBTable, sizeof(district->DBTable), "%s", field[1]);
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_DBTABLEVALUE))) {
    snprintf(district->DBTableValue, sizeof(district->DBTableValue), "%s", field[2]);
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_DATACOUNTY_ID)) && sscanf(field[3], "%" SCNu32, &district->DataCounty_ID) != 1) {
    fprintf(stderr, "Invalid DataCounty_ID.\n");
    return EXIT_FAILURE;
  }

  if (!(district->nullFields &  (1u << DISTRICT_FIELD_DATADISTRICTTOWN_ID)) && sscanf(field[4], "%" SCNu32, &district->DataDistrictTown_ID) != 1) {
    fprintf(stderr, "Invalid DataDistrictTown_ID.\n");
    return EXIT_FAILURE;
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_ELECTORAL)) && sscanf(field[5], "%" SCNu16, &district->Electoral) != 1) {
    fprintf(stderr, "Invalid Electoral value.\n");
    return EXIT_FAILURE;
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_STATEASSEMBLY)) && sscanf(field[6], "%" SCNu16, &district->StateAssembly) != 1) {
    fprintf(stderr, "Invalid StateAssembly value.\n");
    return EXIT_FAILURE;
  }

  unsigned int stateSenate = 0;

  if (!(district->nullFields & (1u << DISTRICT_FIELD_STATESENATE)) && sscanf(field[7], "%u", &stateSenate) != 1) {
    fprintf(stderr, "Invalid StateSenate value.\n");
    return EXIT_FAILURE;
  }

  if (stateSenate > UINT8_MAX) {
    fprintf(stderr, "StateSenate is out of range.\n");
    return EXIT_FAILURE;
  }

  district->StateSenate = (uint8_t)stateSenate;

  if (!(district->nullFields & (1u << DISTRICT_FIELD_LEGISLATIVE)) && sscanf(field[8], "%" SCNu16, &district->Legislative) != 1) {
    fprintf(stderr, "Invalid Legislative value.\n");
    return EXIT_FAILURE;
  }

  if (!(district->nullFields & (1u << DISTRICT_FIELD_WARD))) {
    snprintf(district->Ward, sizeof(district->Ward), "%s", field[9]);
  }

  unsigned int congress = 0;

  if (!(district->nullFields & (1u << DISTRICT_FIELD_CONGRESS)) && sscanf(field[10], "%u", &congress) != 1) {
  	fprintf(stderr, "Invalid Congress value.\n");
    return EXIT_FAILURE;
  }

  if (congress > UINT8_MAX) {
    fprintf(stderr, "Congress is out of range.\n");
    return EXIT_FAILURE;
  }

  district->Congress = (uint8_t)congress;
  return EXIT_SUCCESS;
}


#include "DataDistrict.h"
#include <mysql/mysql.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int QueryDistrictDatabase(
    MYSQL *db,
    const void *record)
{
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid district database arguments.\n");
        return EXIT_FAILURE;
    }

    const DataDistrict *district = record;

    const char *sql =
        "SELECT DataDistrict_ID "
        "FROM DataDistrict "
        "WHERE DataDistrict_ZipCode = ? "
        "AND DataDistrict_DBTable = ? "
        "AND DataDistrict_DBTableValue = ? "
        "AND DataCounty_ID = ? "
        "AND DataDistrictTown_ID = ? "
        "AND DataDistrict_Electoral = ? "
        "AND DataDistrict_StateAssembly = ? "
        "AND DataDistrict_StateSenate = ? "
        "AND DataDistrict_Legislative = ? "
        "AND DataDistrict_Ward = ? "
        "AND DataDistrict_Congress = ?";

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

    unsigned long zipLength =
        (unsigned long)strlen(district->ZipCode);

    unsigned long tableLength =
        (unsigned long)strlen(district->DBTable);

    unsigned long tableValueLength =
        (unsigned long)strlen(district->DBTableValue);

    unsigned long wardLength =
        (unsigned long)strlen(district->Ward);

    MYSQL_BIND params[11];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (void *)district->ZipCode;
    params[0].buffer_length = sizeof(district->ZipCode);
    params[0].length = &zipLength;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (void *)district->DBTable;
    params[1].buffer_length = sizeof(district->DBTable);
    params[1].length = &tableLength;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (void *)district->DBTableValue;
    params[2].buffer_length = sizeof(district->DBTableValue);
    params[2].length = &tableValueLength;

    params[3].buffer_type = MYSQL_TYPE_LONG;
    params[3].buffer = (void *)&district->DataCounty_ID;

    params[4].buffer_type = MYSQL_TYPE_LONG;
    params[4].buffer = (void *)&district->DataDistrictTown_ID;

    params[5].buffer_type = MYSQL_TYPE_SHORT;
    params[5].buffer = (void *)&district->Electoral;

    params[6].buffer_type = MYSQL_TYPE_SHORT;
    params[6].buffer = (void *)&district->StateAssembly;

    params[7].buffer_type = MYSQL_TYPE_TINY;
    params[7].buffer = (void *)&district->StateSenate;

    params[8].buffer_type = MYSQL_TYPE_SHORT;
    params[8].buffer = (void *)&district->Legislative;

    params[9].buffer_type = MYSQL_TYPE_STRING;
    params[9].buffer = (void *)district->Ward;
    params[9].buffer_length = sizeof(district->Ward);
    params[9].length = &wardLength;

    params[10].buffer_type = MYSQL_TYPE_TINY;
    params[10].buffer = (void *)&district->Congress;

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

    uint32_t districtID = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &districtID;
    result.buffer_length = sizeof(districtID);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("District not found in database.\n");

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

    printf("District ID: %" PRIu32 "\n", districtID);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}
