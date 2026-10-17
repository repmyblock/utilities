#include "VoterComplementInfo.h"

struct VoterComplementInfoHashTable { RecordHashTable *records; };
static const RecordField fields[] = {
    {offsetof(VoterComplementInfo, Voters_ID), sizeof(((VoterComplementInfo *)0)->Voters_ID), 0},
    {offsetof(VoterComplementInfo, PrevName), sizeof(((VoterComplementInfo *)0)->PrevName), 1},
    {offsetof(VoterComplementInfo, PrevAddress), sizeof(((VoterComplementInfo *)0)->PrevAddress), 1},
    {offsetof(VoterComplementInfo, DataCountyID_PrevCounty), sizeof(((VoterComplementInfo *)0)->DataCountyID_PrevCounty), 0},
    {offsetof(VoterComplementInfo, LastYearVoted), sizeof(((VoterComplementInfo *)0)->LastYearVoted), 0},
    {offsetof(VoterComplementInfo, LastDateVoted), sizeof(((VoterComplementInfo *)0)->LastDateVoted), 1},
    {offsetof(VoterComplementInfo, OtherParty), sizeof(((VoterComplementInfo *)0)->OtherParty), 1}
};

VoterComplementInfoHashTable *BuildVoterComplementInfoHashTable(const VoterComplementInfo *rows, size_t count) {
  VoterComplementInfoHashTable *table = malloc(sizeof(*table));
  if (!table) return NULL;
  table->records = BuildRecordHashTable(
 		rows, count, sizeof(*rows),
    offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields), fields,
    sizeof(fields)/sizeof(fields[0])
  );
  if (!table->records) { free(table); return NULL; }
  return table;
}

VoterComplementInfoMatch FindVoterComplementInfoID(const VoterComplementInfoHashTable *table, const VoterComplementInfo *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
}

void FreeVoterComplementInfoMatch(VoterComplementInfoMatch *match) { FreeRecordMatch(match); }
void FreeVoterComplementInfoHashTable(VoterComplementInfoHashTable *table) { 
	if (table) {
		FreeRecordHashTable(table->records); free(table); 
	} 
}

#include "RecordLoader.h"
int LoadVoterComplementInfoIntoMemory(MYSQL *db, VoterComplementInfo **rows, size_t *count) {
  if (!rows || !count) return EXIT_FAILURE;
  *rows = NULL; *count = 0;
  void *data = NULL;
  int status = LoadRecordArray(db,
      "SELECT `VotersComplementInfo_ID`, `Voters_ID`, `VotersComplementInfo_PrevName`, " 
      "`VotersComplementInfo_PrevAddress`, `DataCountyID_PrevCounty`, " 
      "`VotersComplementInfo_LastYearVoted`, `VotersComplementInfo_LastDateVoted`, " 
      "`VotersComplementInfo_OtherParty` FROM `VotersComplementInfo`",
      sizeof(VoterComplementInfo), offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields),
      fields, sizeof(fields)/sizeof(fields[0]), &data, count);
  *rows = data;
  return status;
}

int QueryVoterComplementInfoMemory(const void *hashTable, const void *record) {
  VoterComplementInfoMatch match = FindVoterComplementInfoID(hashTable, record);

  if (match.status == VOTER_COMPLEMENT_INFO_NOT_FOUND) {
     printf("Voter complement information not found in memory.\n");
  } else if (match.status == VOTER_COMPLEMENT_INFO_ERROR) {
     fprintf(stderr, "Voter complement-information lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
     printf("Voter complement-information ID: %" PRIu32 "\n", match.IDs[i]);
    }
  }

  int result = match.status == VOTER_COMPLEMENT_INFO_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;

  FreeVoterComplementInfoMatch(&match);
  return result;
}

static int IsValidISODate(const char *date) {
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

int ParseVoterComplementInfoQuery(
    char *line,
    void *output)
{
    if (line == NULL || output == NULL) {
        return EXIT_FAILURE;
    }

    VoterComplementInfo *info =
        (VoterComplementInfo *)output;

    memset(info, 0, sizeof(*info));

    char *field[7] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 7) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 7 || token != NULL) {
        fprintf(stderr,
                "Voter-complement format:\n"
                "Voters_ID,PrevName,PrevAddress,"
                "DataCountyID_PrevCounty,LastYearVoted,"
                "LastDateVoted,OtherParty\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < 7; ++i) {
        if (field[i][0] == '\0') {
            info->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_VOTERS_ID)) &&
        sscanf(field[0],
               "%" SCNu32,
               &info->Voters_ID) != 1) {
        fprintf(stderr, "Invalid Voters_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_PREVNAME))) {
        snprintf(
            info->PrevName,
            sizeof(info->PrevName),
            "%s",
            field[1]);
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_PREVADDRESS))) {
        snprintf(
            info->PrevAddress,
            sizeof(info->PrevAddress),
            "%s",
            field[2]);
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_DATACOUNTYID_PREVCOUNTY)) &&
        sscanf(field[3],
               "%" SCNu32,
               &info->DataCountyID_PrevCounty) != 1) {
        fprintf(stderr,
                "Invalid DataCountyID_PrevCounty.\n");
        return EXIT_FAILURE;
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_LASTYEARVOTED))) {
        unsigned int year = 0;

        if (sscanf(field[4], "%u", &year) != 1 ||
            year > UINT16_MAX) {
            fprintf(stderr,
                    "Invalid LastYearVoted.\n");
            return EXIT_FAILURE;
        }

        info->LastYearVoted =
            (uint16_t)year;
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_LASTDATEVOTED))) {
        if (!IsValidISODate(field[5])) {
            fprintf(stderr,
                    "Invalid LastDateVoted. "
                    "Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        snprintf(
            info->LastDateVoted,
            sizeof(info->LastDateVoted),
            "%s",
            field[5]);
    }

    if (!(info->nullFields &
          (1u << VOTER_COMPLEMENT_INFO_FIELD_OTHERPARTY))) {
        snprintf(
            info->OtherParty,
            sizeof(info->OtherParty),
            "%s",
            field[6]);
    }

    return EXIT_SUCCESS;
}

int QueryVoterComplementInfoDatabase(
    MYSQL *db,
    const void *record)
{
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid voter-complement-info arguments.\n");
        return EXIT_FAILURE;
    }

    const VoterComplementInfo *info = record;

    const char *sql =
        "SELECT VotersComplementInfo_ID "
        "FROM VotersComplementInfo "
        "WHERE Voters_ID = ? "
        "AND VotersComplementInfo_PrevName = ? "
        "AND VotersComplementInfo_PrevAddress = ? "
        "AND DataCountyID_PrevCounty = ? "
        "AND VotersComplementInfo_LastYearVoted = ? "
        "AND VotersComplementInfo_LastDateVoted = ? "
        "AND VotersComplementInfo_OtherParty = ?";

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

    unsigned long prevNameLength =
        (unsigned long)strlen(info->PrevName);

    unsigned long prevAddressLength =
        (unsigned long)strlen(info->PrevAddress);

    unsigned long dateLength =
        (unsigned long)strlen(info->LastDateVoted);

    unsigned long otherPartyLength =
        (unsigned long)strlen(info->OtherParty);

    MYSQL_BIND params[7];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&info->Voters_ID;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (void *)info->PrevName;
    params[1].buffer_length = sizeof(info->PrevName);
    params[1].length = &prevNameLength;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (void *)info->PrevAddress;
    params[2].buffer_length = sizeof(info->PrevAddress);
    params[2].length = &prevAddressLength;

    params[3].buffer_type = MYSQL_TYPE_LONG;
    params[3].buffer = (void *)&info->DataCountyID_PrevCounty;

    params[4].buffer_type = MYSQL_TYPE_SHORT;
    params[4].buffer = (void *)&info->LastYearVoted;

    params[5].buffer_type = MYSQL_TYPE_STRING;
    params[5].buffer = (void *)info->LastDateVoted;
    params[5].buffer_length = sizeof(info->LastDateVoted);
    params[5].length = &dateLength;

    params[6].buffer_type = MYSQL_TYPE_STRING;
    params[6].buffer = (void *)info->OtherParty;
    params[6].buffer_length = sizeof(info->OtherParty);
    params[6].length = &otherPartyLength;

    if (mysql_stmt_bind_param(stmt, params) != 0) {
        fprintf(stderr, "Bind failed: %s\n",
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

    uint32_t id = 0;

    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &id;
    result.buffer_length = sizeof(id);

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("Voter complement information not found in database.\n");

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

    printf("Voter complement information ID: %" PRIu32 "\n", id);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}