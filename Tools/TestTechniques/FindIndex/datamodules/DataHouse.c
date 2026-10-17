#include "DataHouse.h"
#include "RecordLoader.h"

#include <stdlib.h>
struct HouseHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
  {offsetof(DataHouse, DataAddress_ID), sizeof(((DataHouse *)0)->DataAddress_ID), 0},
  {offsetof(DataHouse, Type), sizeof(((DataHouse *)0)->Type), 1},
  {offsetof(DataHouse, Apt), sizeof(((DataHouse *)0)->Apt), 1},
  {offsetof(DataHouse, DataStreetNonStdFormat_ID), sizeof(((DataHouse *)0)->DataStreetNonStdFormat_ID), 0},
  {offsetof(DataHouse, BIN), sizeof(((DataHouse *)0)->BIN), 0}
};

HouseHashTable *BuildHouseHashTable(const DataHouse *rows, size_t count) {
	HouseHashTable *table = malloc(sizeof(*table));
	if (!table) return NULL;
	table->records = BuildRecordHashTable(
		rows, count, sizeof(*rows),
	  offsetof(DataHouse, ID), offsetof(DataHouse, nullFields), fields,
	  sizeof(fields)/sizeof(fields[0])
	);
	if (!table->records) { free(table); return NULL; }
	return table;
}

void FreeHouseMatch(HouseMatch *match) { FreeRecordMatch(match); }
void FreeHouseHashTable(HouseHashTable *table) { 
	if (table) { 
		FreeRecordHashTable(table->records); free(table); 
	} 
}

int LoadHouseIntoMemory(MYSQL *db, DataHouse **rows, size_t *count) {
  if (!rows || !count) return EXIT_FAILURE;
  *rows = NULL; *count = 0;
  void *data = NULL;
  int status = LoadRecordArray(db,
      "SELECT `DataHouse_ID`, `DataAddress_ID`, `DataHouse_Type`, `DataHouse_Apt`, " 
      "`DataStreetNonStdFormat_ID`, `DataHouse_BIN` FROM `DataHouse`",
      sizeof(DataHouse), offsetof(DataHouse, ID), offsetof(DataHouse, nullFields),
      fields, sizeof(fields)/sizeof(fields[0]), &data, count);
  *rows = data;
  return status;
}



/* DataHouse.c */

/*
HouseMatch FindHouseID(const HouseHashTable *table, const DataHouse *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
} 
*/

HouseMatch FindHouseID(const HouseHashTable *table, const DataHouse *query) {
	if (table == NULL || query == NULL) {
    HouseMatch result = { .status = HOUSE_ERROR, .IDs = NULL, .count = 0 };
    return result;
	}
	return FindRecordID(table->records, query);
}

int QueryHouseMemory(const void *hashTable, const void *record) {
  HouseMatch match = FindHouseID(hashTable, record);

  if (match.status == HOUSE_NOT_FOUND) {
	  printf("House not found in memory.\n");
  } else if (match.status == HOUSE_ERROR) {
    fprintf(stderr, "House lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
      printf("House ID: %" PRIu32 "\n", match.IDs[i]);
    }
	}

  int result = match.status == HOUSE_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;

  FreeHouseMatch(&match);
  return result;
}

int ParseHouseQuery(char *line, void *output) {
	if (line == NULL || output == NULL) {
		return EXIT_FAILURE;
	}

    DataHouse *house = (DataHouse *)output;

    memset(house, 0, sizeof(*house));

    char *field[5] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 5) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 5 || token != NULL) {
        fprintf(stderr,
                "House format:\n"
                "DataAddress_ID,Type,Apt,"
                "DataStreetNonStdFormat_ID,BIN\n");
        return EXIT_FAILURE;
    }

    /*
     * Empty fields are SQL NULL.
     */
    for (size_t i = 0; i < 5; ++i) {
        if (field[i][0] == '\0') {
            house->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

    if (!(house->nullFields &
          (1u << HOUSE_FIELD_DATAADDRESS_ID))) {
        if (sscanf(field[0],
                   "%" SCNu32,
                   &house->DataAddress_ID) != 1) {
            fprintf(stderr,
                    "Invalid DataAddress_ID.\n");
            return EXIT_FAILURE;
        }
    }

    if (!(house->nullFields &
          (1u << HOUSE_FIELD_TYPE))) {
        snprintf(
            house->Type,
            sizeof(house->Type),
            "%s",
            field[1]);
    }

    if (!(house->nullFields &
          (1u << HOUSE_FIELD_APT))) {
        snprintf(
            house->Apt,
            sizeof(house->Apt),
            "%s",
            field[2]);
    }

    if (!(house->nullFields &
          (1u << HOUSE_FIELD_DATASTREETNONSTDFORMAT_ID))) {
        if (sscanf(field[3],
                   "%" SCNu32,
                   &house->DataStreetNonStdFormat_ID) != 1) {
            fprintf(stderr,
                    "Invalid DataStreetNonStdFormat_ID.\n");
            return EXIT_FAILURE;
        }
    }

    if (!(house->nullFields &
          (1u << HOUSE_FIELD_BIN))) {
        if (sscanf(field[4],
                   "%" SCNu32,
                   &house->BIN) != 1) {
            fprintf(stderr,
                    "Invalid BIN.\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}