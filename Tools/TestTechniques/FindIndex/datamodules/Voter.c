#include "Voter.h"
#include <stdlib.h>
struct VoterHashTable { RecordHashTable *records; };

static const RecordField fields[] = {
  {offsetof(Voter, VotersIndexes_ID), sizeof(((Voter *)0)->VotersIndexes_ID), 0},
  {offsetof(Voter, DataHouse_ID), sizeof(((Voter *)0)->DataHouse_ID), 0},
  {offsetof(Voter, Gender), sizeof(((Voter *)0)->Gender), 1},
  {offsetof(Voter, UniqStateVoterID), sizeof(((Voter *)0)->UniqStateVoterID), 1},
  {offsetof(Voter, DataState_ID), sizeof(((Voter *)0)->DataState_ID), 0},
  {offsetof(Voter, RegParty), sizeof(((Voter *)0)->RegParty), 1},
  {offsetof(Voter, ReasonCode), sizeof(((Voter *)0)->ReasonCode), 1},
  {offsetof(Voter, Status), sizeof(((Voter *)0)->Status), 1},
  {offsetof(Voter, VotersMailingAddress_ID), sizeof(((Voter *)0)->VotersMailingAddress_ID), 0},
  {offsetof(Voter, IDRequired), sizeof(((Voter *)0)->IDRequired), 1},
  {offsetof(Voter, IDMet), sizeof(((Voter *)0)->IDMet), 1},
  {offsetof(Voter, ApplyDate), sizeof(((Voter *)0)->ApplyDate), 1},
  {offsetof(Voter, RegSource), sizeof(((Voter *)0)->RegSource), 1},
  {offsetof(Voter, DateInactive), sizeof(((Voter *)0)->DateInactive), 1},
  {offsetof(Voter, DatePurged), sizeof(((Voter *)0)->DatePurged), 1},
  {offsetof(Voter, CountyVoterNumber), sizeof(((Voter *)0)->CountyVoterNumber), 1},
  {offsetof(Voter, RMBActive), sizeof(((Voter *)0)->RMBActive), 1},
  {offsetof(Voter, RecFirstSeen), sizeof(((Voter *)0)->RecFirstSeen), 1},
  {offsetof(Voter, RecLastSeen), sizeof(((Voter *)0)->RecLastSeen), 1}
};

VoterHashTable *BuildVoterHashTable(const Voter *rows, size_t count) {
  VoterHashTable *table = malloc(sizeof(*table));
  if (!table) return NULL;
  table->records = BuildRecordHashTable(
  	rows, count, sizeof(*rows), offsetof(Voter, ID), offsetof(Voter, nullFields), 
  	fields,  sizeof(fields)/sizeof(fields[0])
 	);
  if (!table->records) { free(table); return NULL; }
  return table;
}



void FreeVoterMatch(VoterMatch *match) { FreeRecordMatch(match); }

void FreeVoterHashTable(VoterHashTable *table) { 
	if (table) { FreeRecordHashTable(table->records); free(table); } 
}

#include "RecordLoader.h"
int LoadVoterIntoMemory(MYSQL *db, Voter **rows, size_t *count) {
  if (!rows || !count) return EXIT_FAILURE;
  *rows = NULL; *count = 0;
  void *data = NULL;
  int status = LoadRecordArray(db,
      "SELECT `Voters_ID`, `VotersIndexes_ID`, `DataHouse_ID`, `Voters_Gender`, " 
      "`Voters_UniqStateVoterID`, `DataState_ID`, `Voters_RegParty`, " 
      "`Voters_ReasonCode`, `Voters_Status`, `VotersMailingAddress_ID`, `Voters_IDRequired`, " 
      "`Voters_IDMet`, `Voters_ApplyDate`, `Voters_RegSource`, `Voters_DateInactive`, " 
      "`Voters_DatePurged`, `Voters_CountyVoterNumber`, `Voters_RMBActive`, " 
      "`Voters_RecFirstSeen`, `Voters_RecLastSeen` FROM `Voters`",
      sizeof(Voter), offsetof(Voter, ID), offsetof(Voter, nullFields),
      fields, sizeof(fields)/sizeof(fields[0]), &data, count);
  *rows = data;
  return status;
}

/* Voter.c */

/*
VoterMatch FindVoterID(const VoterHashTable *table, const Voter *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
}
*/

VoterMatch FindVoterID(const VoterHashTable *table, const Voter *query) {
	if (table == NULL || query == NULL) {
    VoterMatch result = { .status = VOTER_ERROR, .IDs = NULL, .count = 0 };
    return result;
	}
	return FindRecordID(table->records, query);
}

int QueryVoterMemory(const void *hashTable, const void *record) {
  VoterMatch match = FindVoterID(hashTable, record);
  if (match.status == VOTER_NOT_FOUND) {
  	printf("Voter not found in memory.\n");
  } else if (match.status == VOTER_ERROR) {
    fprintf(stderr, "Voter lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
  	  printf("Voter ID: %" PRIu32 "\n", match.IDs[i]);
    }
	}
  int result = match.status == VOTER_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;
  FreeVoterMatch(&match);
  return result;
}

static int IsValidISODate(
    const char *date)
{
    if (date == NULL ||
        strlen(date) != 10 ||
        date[4] != '-' ||
        date[7] != '-') {
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

    return 1;
}

int ParseVoterQuery(
    char *line,
    void *output)
{
    if (line == NULL || output == NULL) {
        return EXIT_FAILURE;
    }

    Voter *voter = output;
    memset(voter, 0, sizeof(*voter));

    char *field[19] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 19) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 19 || token != NULL) {
        fprintf(stderr,
            "Voter format requires 19 comma-separated fields.\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < 19; ++i) {
        if (field[i][0] == '\0') {
            voter->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

#define PARSE_U32(FIELD, ENUM_VALUE, NAME)                         \
    do {                                                           \
        if (!(voter->nullFields & (1u << (ENUM_VALUE))) &&        \
            sscanf(field[(FIELD)], "%" SCNu32, &(NAME)) != 1) {   \
            fprintf(stderr, "Invalid " #NAME ".\n");              \
            return EXIT_FAILURE;                                  \
        }                                                          \
    } while (0)

#define COPY_TEXT(FIELD, ENUM_VALUE, DEST)                         \
    do {                                                           \
        if (!(voter->nullFields & (1u << (ENUM_VALUE)))) {        \
            snprintf((DEST), sizeof(DEST), "%s", field[(FIELD)]); \
        }                                                          \
    } while (0)

    PARSE_U32(
        0,
        VOTER_FIELD_VOTERSINDEXES_ID,
        voter->VotersIndexes_ID);

    PARSE_U32(
        1,
        VOTER_FIELD_DATAHOUSE_ID,
        voter->DataHouse_ID);

    COPY_TEXT(
        2,
        VOTER_FIELD_GENDER,
        voter->Gender);

    COPY_TEXT(
        3,
        VOTER_FIELD_UNIQSTATEVOTERID,
        voter->UniqStateVoterID);

    PARSE_U32(
        4,
        VOTER_FIELD_DATASTATE_ID,
        voter->DataState_ID);

    COPY_TEXT(
        5,
        VOTER_FIELD_REGPARTY,
        voter->RegParty);

    COPY_TEXT(
        6,
        VOTER_FIELD_REASONCODE,
        voter->ReasonCode);

    COPY_TEXT(
        7,
        VOTER_FIELD_STATUS,
        voter->Status);

    PARSE_U32(
        8,
        VOTER_FIELD_VOTERSMAILINGADDRESS_ID,
        voter->VotersMailingAddress_ID);

    COPY_TEXT(
        9,
        VOTER_FIELD_IDREQUIRED,
        voter->IDRequired);

    COPY_TEXT(
        10,
        VOTER_FIELD_IDMET,
        voter->IDMet);

    if (!(voter->nullFields &
          (1u << VOTER_FIELD_APPLYDATE))) {
        if (!IsValidISODate(field[11])) {
            fprintf(stderr,
                    "Invalid ApplyDate. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        COPY_TEXT(
            11,
            VOTER_FIELD_APPLYDATE,
            voter->ApplyDate);
    }

    COPY_TEXT(
        12,
        VOTER_FIELD_REGSOURCE,
        voter->RegSource);

    if (!(voter->nullFields &
          (1u << VOTER_FIELD_DATEINACTIVE))) {
        if (!IsValidISODate(field[13])) {
            fprintf(stderr,
                    "Invalid DateInactive. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        COPY_TEXT(
            13,
            VOTER_FIELD_DATEINACTIVE,
            voter->DateInactive);
    }

    if (!(voter->nullFields &
          (1u << VOTER_FIELD_DATEPURGED))) {
        if (!IsValidISODate(field[14])) {
            fprintf(stderr,
                    "Invalid DatePurged. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        COPY_TEXT(
            14,
            VOTER_FIELD_DATEPURGED,
            voter->DatePurged);
    }

    COPY_TEXT(
        15,
        VOTER_FIELD_COUNTYVOTERNUMBER,
        voter->CountyVoterNumber);

    COPY_TEXT(
        16,
        VOTER_FIELD_RMBACTIVE,
        voter->RMBActive);

    if (!(voter->nullFields &
          (1u << VOTER_FIELD_RECFIRSTSEEN))) {
        if (!IsValidISODate(field[17])) {
            fprintf(stderr,
                    "Invalid RecFirstSeen. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        COPY_TEXT(
            17,
            VOTER_FIELD_RECFIRSTSEEN,
            voter->RecFirstSeen);
    }

    if (!(voter->nullFields &
          (1u << VOTER_FIELD_RECLASTSEEN))) {
        if (!IsValidISODate(field[18])) {
            fprintf(stderr,
                    "Invalid RecLastSeen. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        COPY_TEXT(
            18,
            VOTER_FIELD_RECLASTSEEN,
            voter->RecLastSeen);
    }

#undef PARSE_U32
#undef COPY_TEXT

    return EXIT_SUCCESS;
}


#include "Voter.h"
#include <mysql/mysql.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int QueryVoterDatabase(MYSQL *db, const void *record)
{
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid voter database arguments.\n");
        return EXIT_FAILURE;
    }

    const Voter *voter = record;

    /*
     * Search using the identifying voter fields.
     * Voters_ID is returned when a matching record is found.
     */
    const char *sql =
        "SELECT Voters_ID "
        "FROM Voters "
        "WHERE VotersIndexes_ID = ? "
        "AND DataHouse_ID = ? "
        "AND Voters_UniqStateVoterID = ? "
        "AND DataState_ID = ? "
        "AND Voters_CountyVoterNumber = ?";

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

    unsigned long stateVoterIDLength =
        (unsigned long)strlen(voter->UniqStateVoterID);

    unsigned long countyVoterNumberLength =
        (unsigned long)strlen(voter->CountyVoterNumber);

    MYSQL_BIND params[5];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&voter->VotersIndexes_ID;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = (void *)&voter->DataHouse_ID;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (void *)voter->UniqStateVoterID;
    params[2].buffer_length = sizeof(voter->UniqStateVoterID);
    params[2].length = &stateVoterIDLength;

    params[3].buffer_type = MYSQL_TYPE_LONG;
    params[3].buffer = (void *)&voter->DataState_ID;

    params[4].buffer_type = MYSQL_TYPE_STRING;
    params[4].buffer = (void *)voter->CountyVoterNumber;
    params[4].buffer_length = sizeof(voter->CountyVoterNumber);
    params[4].length = &countyVoterNumberLength;

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

    uint32_t voterID = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &voterID;
    result.buffer_length = sizeof(voterID);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("Voter not found in database.\n");

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

    printf("Voter ID: %" PRIu32 "\n", voterID);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}

#include "DataHouse.h"
#include <mysql/mysql.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int QueryHouseDatabase(MYSQL *db, const void *record) {
	if (db == NULL || record == NULL) {
	  fprintf(stderr, "Invalid house database arguments.\n");
	  return EXIT_FAILURE;
	}

  const DataHouse *house = record;

  const char *sql =
        "SELECT DataHouse_ID "
        "FROM DataHouse "
        "WHERE DataAddress_ID = ? "
        "AND DataHouse_Type = ? "
        "AND DataHouse_Apt = ? "
        "AND DataStreetNonStdFormat_ID = ? "
        "AND DataHouse_BIN = ?";

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

    unsigned long typeLength =
        (unsigned long)strlen(house->Type);

    unsigned long aptLength =
        (unsigned long)strlen(house->Apt);

    MYSQL_BIND params[5];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&house->DataAddress_ID;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (void *)house->Type;
    params[1].buffer_length = sizeof(house->Type);
    params[1].length = &typeLength;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (void *)house->Apt;
    params[2].buffer_length = sizeof(house->Apt);
    params[2].length = &aptLength;

    params[3].buffer_type = MYSQL_TYPE_LONG;
    params[3].buffer = (void *)&house->DataStreetNonStdFormat_ID;

    params[4].buffer_type = MYSQL_TYPE_LONG;
    params[4].buffer = (void *)&house->BIN;

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

    uint32_t houseID = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &houseID;
    result.buffer_length = sizeof(houseID);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("House not found in database.\n");

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

    printf("House ID: %" PRIu32 "\n", houseID);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}