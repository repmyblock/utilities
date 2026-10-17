#include "DataDistrictCycle.h"

struct DistrictCycleHashTable { RecordHashTable *records; };

static const RecordField fields[] = {
  {offsetof(DataDistrictCycle, CycleStartDate), sizeof(((DataDistrictCycle *)0)->CycleStartDate), 1},
  {offsetof(DataDistrictCycle, CycleEndDate), sizeof(((DataDistrictCycle *)0)->CycleEndDate), 1}
};

DistrictCycleHashTable *BuildDistrictCycleHashTable(const DataDistrictCycle *rows, size_t count) {
  DistrictCycleHashTable *table = malloc(sizeof(*table));
  if (!table) return NULL;
  table->records = BuildRecordHashTable(
  	rows, count, sizeof(*rows),
    offsetof(DataDistrictCycle, ID), offsetof(DataDistrictCycle, nullFields), fields,
    sizeof(fields)/sizeof(fields[0])
  );
  if (!table->records) { free(table); return NULL; }
  return table;
}

DistrictCycleMatch FindDistrictCycleID(const DistrictCycleHashTable *table, const DataDistrictCycle *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
}

void FreeDistrictCycleMatch(DistrictCycleMatch *match) { FreeRecordMatch(match); }
void FreeDistrictCycleHashTable(DistrictCycleHashTable *table) { 
	if (table) {
		FreeRecordHashTable(table->records); free(table); 
	} 
}

int LoadDistrictCycleIntoMemory(MYSQL *db, DataDistrictCycle **rows, size_t *count) {
  if (!rows || !count) return EXIT_FAILURE;
  *rows = NULL; *count = 0;
  void *data = NULL;
  int status = LoadRecordArray(db,
      "SELECT `DataDistrictCycle_ID`, `DataDistrictCycle_CycleStartDate`, " 
      "`DataDistrictCycle_CycleEndDate` FROM `DataDistrictCycle`",
      sizeof(DataDistrictCycle), offsetof(DataDistrictCycle, ID), offsetof(DataDistrictCycle, nullFields),
      fields, sizeof(fields)/sizeof(fields[0]), &data, count);
  *rows = data;
  return status;
}


static int IsValidISODate(const char *date) {
	if (date == NULL || strlen(date) != 10 || 	date[4] != '-' || date[7] != '-') {
		return 0;
	}

	for (size_t i = 0; i < 10; ++i) {
		if (i == 4 || i == 7) {
		  continue;
		}

		if (!isdigit((unsigned char)date[i])) {
		  return 0;
		}
	}

	unsigned year =	(unsigned)strtoul(date, NULL, 10);
	unsigned month = (unsigned)strtoul(date + 5, NULL, 10);
	unsigned day = (unsigned)strtoul(date + 8, NULL, 10);

	if (year == 0 || month < 1 || month > 12 ||	day < 1 || day > 31) {
		return 0;
	}

	return 1;
}


int ParseDistrictCycleQuery(char *line, void *output) {
	if (line == NULL || output == NULL) {
	  return EXIT_FAILURE;
	}

	DataDistrictCycle *cycle = (DataDistrictCycle *)output;
	memset(cycle, 0, sizeof(*cycle));

	char *startDate = strtok(line, ",");
	char *endDate = strtok(NULL, ",");

	if (startDate == NULL || endDate == NULL ||
	  strtok(NULL, ",") != NULL) {
	  fprintf(stderr, "District-cycle format must be:\nCycleStartDate,CycleEndDate\n");
	  return EXIT_FAILURE;
	}

	/*
	* Empty fields represent SQL NULL.
	*/
	if (startDate[0] == '\0') {
	  cycle->nullFields |= (uint32_t)1u << DISTRICT_CYCLE_FIELD_CYCLESTARTDATE;
	} else {
	  if (!IsValidISODate(startDate)) {
      fprintf(stderr, "Invalid CycleStartDate. Use YYYY-MM-DD.\n");
      return EXIT_FAILURE;
	  }
	  snprintf(cycle->CycleStartDate, sizeof(cycle->CycleStartDate), "%s", startDate);
	}

	if (endDate[0] == '\0') {
	  cycle->nullFields |= (uint32_t)1u << DISTRICT_CYCLE_FIELD_CYCLEENDDATE;
	} else {
	  if (!IsValidISODate(endDate)) {
      fprintf(stderr, "Invalid CycleEndDate. Use YYYY-MM-DD.\n");
      return EXIT_FAILURE;
	  }
	  snprintf(cycle->CycleEndDate, sizeof(cycle->CycleEndDate), "%s", endDate);
	}
	return EXIT_SUCCESS;
}


#include "DataDistrictCycle.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int QueryDistrictCycleMemory(
    const void *hashTable,
    const void *record)
{
    if (hashTable == NULL || record == NULL) {
        fprintf(stderr,
                "Invalid district-cycle lookup arguments.\n");
        return EXIT_FAILURE;
    }

    const DistrictCycleHashTable *table =
        (const DistrictCycleHashTable *)hashTable;

    const DataDistrictCycle *cycle =
        (const DataDistrictCycle *)record;

    DistrictCycleMatch match =
        FindDistrictCycleID(table, cycle);

    if (match.status == DISTRICT_CYCLE_NOT_FOUND) {
        printf("District cycle not found in memory.\n");
    } else if (match.status == DISTRICT_CYCLE_ERROR) {
        fprintf(stderr,
                "District-cycle lookup failed.\n");
    } else {
        if (match.status == DISTRICT_CYCLE_DUPLICATE) {
            printf("Duplicate district-cycle records: %zu\n",
                   match.count);
        }

        for (size_t i = 0; i < match.count; ++i) {
            printf("District-cycle ID: %" PRIu32 "\n",
                   match.IDs[i]);
        }
    }

    int result =
        match.status == DISTRICT_CYCLE_ERROR
            ? EXIT_FAILURE
            : EXIT_SUCCESS;

    FreeDistrictCycleMatch(&match);
    return result;
}

int QueryDistrictCycleDatabase(MYSQL *db, const void *record) {
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid district-cycle database arguments.\n");
        return EXIT_FAILURE;
    }

    const DataDistrictCycle *cycle = record;

    const char *sql =
        "SELECT DataDistrictCycle_ID "
        "FROM DataDistrictCycle "
        "WHERE DataDistrictCycle_CycleStartDate = ? "
        "AND DataDistrictCycle_CycleEndDate = ?";

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

    unsigned long startDateLength =
        (unsigned long)strlen(cycle->CycleStartDate);

    unsigned long endDateLength =
        (unsigned long)strlen(cycle->CycleEndDate);

    MYSQL_BIND params[2];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (void *)cycle->CycleStartDate;
    params[0].buffer_length = sizeof(cycle->CycleStartDate);
    params[0].length = &startDateLength;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (void *)cycle->CycleEndDate;
    params[1].buffer_length = sizeof(cycle->CycleEndDate);
    params[1].length = &endDateLength;

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

    uint32_t cycleID = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &cycleID;
    result.buffer_length = sizeof(cycleID);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("District cycle not found in database.\n");

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

    printf("District cycle ID: %" PRIu32 "\n", cycleID);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}
