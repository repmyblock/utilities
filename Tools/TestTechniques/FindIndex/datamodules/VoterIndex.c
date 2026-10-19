#include "VoterIndex.h"
#include "RecordLoader.h"

struct VoterIndexHashTable { RecordHashTable *records; };

static const RecordField fields[] = {
  {offsetof(VoterIndex, DataLastName_ID), sizeof(((VoterIndex *)0)->DataLastName_ID), 0},
  {offsetof(VoterIndex, DataFirstName_ID), sizeof(((VoterIndex *)0)->DataFirstName_ID), 0},
  {offsetof(VoterIndex, DataMiddleName_ID), sizeof(((VoterIndex *)0)->DataMiddleName_ID), 0},
  {offsetof(VoterIndex, Suffix), sizeof(((VoterIndex *)0)->Suffix), 1},
  {offsetof(VoterIndex, DOB), sizeof(((VoterIndex *)0)->DOB), 1},
  {offsetof(VoterIndex, DataState_ID), sizeof(((VoterIndex *)0)->DataState_ID), 0},
  {offsetof(VoterIndex, UniqStateVoterID), sizeof(((VoterIndex *)0)->UniqStateVoterID), 1}
};

VoterIndexHashTable *BuildVoterIndexHashTable(const VoterIndex *rows, size_t count) {
  VoterIndexHashTable *table = malloc(sizeof(*table));
  if (!table) return NULL;
  table->records = BuildRecordHashTable(rows, count, sizeof(*rows),
      offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields), fields,
      sizeof(fields)/sizeof(fields[0]));
  if (!table->records) { free(table); return NULL; }
  return table;
}

VoterIndexMatch FindVoterIndexID(const VoterIndexHashTable *table, const VoterIndex *query) { 
	return FindRecordID(table ? table->records : NULL, query); 
}

void FreeVoterIndexMatch(VoterIndexMatch *match) { 
	FreeRecordMatch(match); 
}

void FreeVoterIndexHashTable(VoterIndexHashTable *table) { 
	if (table) { 
		FreeRecordHashTable(table->records); 
		free(table); 
	} 
}

int LoadVoterIndexIntoMemory(MYSQL *db, VoterIndex **rows, size_t *count, size_t limit) {
  if (rows == NULL || count == NULL) return EXIT_FAILURE;
  *rows = NULL;
  *count = 0;

  if (db == NULL) return EXIT_FAILURE;

  const char *select = "SELECT VotersIndexes_ID, DataLastName_ID, DataFirstName_ID, DataMiddleName_ID, "
								        "VotersIndexes_Suffix, VotersIndexes_DOB, DataState_ID, VotersIndexes_UniqStateVoterID "
								        "FROM VotersIndexes";
  char query[512];
  int queryLength;
  if (limit == 0) queryLength = snprintf(query, sizeof(query), "%s", select);
  else queryLength = snprintf(query, sizeof(query), "%s LIMIT %zu", select, limit);

  if (queryLength < 0 || (size_t)queryLength >= sizeof(query)) {
    fprintf(stderr, "Voter-index query is too long.\n");
    return EXIT_FAILURE;
  }

  void *data = NULL;
  int status = LoadRecordArray(db, query, sizeof(VoterIndex), offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields),
     														fields, sizeof(fields) / sizeof(fields[0]), &data, count);

  if (status != EXIT_SUCCESS) {
    free(data);
    *count = 0;
    return status;
  }

  *rows = data;
  return EXIT_SUCCESS;
}

int QueryVoterIndexMemory(const void *hashTable, const void *record) {
  VoterIndexMatch match = FindVoterIndexID(hashTable, record);
  
  if (match.status == VOTER_INDEX_NOT_FOUND) {
    printf("Voter index not found in memory.\n");
  } else if (match.status == VOTER_INDEX_ERROR) {
    fprintf(stderr, "Voter-index lookup failed.\n");
  } else {
    for (size_t i = 0; i < match.count; ++i) {
      printf("Voter index ID: %" PRIu32 "\n", match.IDs[i]);
    }
  }

  int result = match.status == VOTER_INDEX_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;

  FreeVoterIndexMatch(&match);
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


int ParseVoterIndexQuery(char *line, void *output) {
    if (line == NULL || output == NULL) {
        return EXIT_FAILURE;
    }

    VoterIndex *index = (VoterIndex *)output;

    memset(index, 0, sizeof(*index));

    char *field[7] = {0};
    size_t count = 0;

    char *token = strtok(line, ",");

    while (token != NULL && count < 7) {
        field[count++] = token;
        token = strtok(NULL, ",");
    }

    if (count != 7 || token != NULL) {
        fprintf(stderr,
                "Voter-index format:\n"
                "DataLastName_ID,DataFirstName_ID,"
                "DataMiddleName_ID,Suffix,DOB,"
                "DataState_ID,UniqStateVoterID\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < 7; ++i) {
        if (field[i][0] == '\0') {
            index->nullFields |=
                (uint32_t)1u << (i + 1);
        }
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_DATALASTNAME_ID)) &&
        sscanf(field[0],
               "%" SCNu32,
               &index->DataLastName_ID) != 1) {
        fprintf(stderr,
                "Invalid DataLastName_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_DATAFIRSTNAME_ID)) &&
        sscanf(field[1],
               "%" SCNu32,
               &index->DataFirstName_ID) != 1) {
        fprintf(stderr,
                "Invalid DataFirstName_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_DATAMIDDLENAME_ID)) &&
        sscanf(field[2],
               "%" SCNu32,
               &index->DataMiddleName_ID) != 1) {
        fprintf(stderr,
                "Invalid DataMiddleName_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_SUFFIX))) {
        snprintf(
            index->Suffix,
            sizeof(index->Suffix),
            "%s",
            field[3]);
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_DOB))) {
        if (!IsValidISODate(field[4])) {
            fprintf(stderr,
                    "Invalid DOB. Use YYYY-MM-DD.\n");
            return EXIT_FAILURE;
        }

        snprintf(
            index->DOB,
            sizeof(index->DOB),
            "%s",
            field[4]);
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_DATASTATE_ID)) &&
        sscanf(field[5],
               "%" SCNu32,
               &index->DataState_ID) != 1) {
        fprintf(stderr,
                "Invalid DataState_ID.\n");
        return EXIT_FAILURE;
    }

    if (!(index->nullFields &
          (1u << VOTER_INDEX_FIELD_UNIQSTATEVOTERID))) {
        snprintf(
            index->UniqStateVoterID,
            sizeof(index->UniqStateVoterID),
            "%s",
            field[6]);
    }

    return EXIT_SUCCESS;
}


int QueryVoterIndexDatabase(MYSQL *db, const void *record)
{
    if (db == NULL || record == NULL) {
        fprintf(stderr, "Invalid voter-index database arguments.\n");
        return EXIT_FAILURE;
    }

    const VoterIndex *v = record;

    const char *sql =
        "SELECT VotersIndexes_ID "
        "FROM VotersIndexes "
        "WHERE DataLastName_ID = ? "
        "AND DataFirstName_ID = ? "
        "AND DataMiddleName_ID = ? "
        "AND VotersIndexes_Suffix = ? "
        "AND VotersIndexes_DOB = ? "
        "AND DataState_ID = ? "
        "AND VotersIndexes_UniqStateVoterID = ?";

    MYSQL_STMT *stmt = mysql_stmt_init(db);

    if (stmt == NULL) {
        fprintf(stderr, "Statement initialization failed: %s\n",
                mysql_error(db));
        return EXIT_FAILURE;
    }

    if (mysql_stmt_prepare(stmt, sql, (unsigned long)strlen(sql)) != 0) {
        fprintf(stderr, "Prepare failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    unsigned long suffixLength =
        (unsigned long)strlen(v->Suffix);

    unsigned long dobLength =
        (unsigned long)strlen(v->DOB);

    unsigned long voterIdLength =
        (unsigned long)strlen(v->UniqStateVoterID);

    MYSQL_BIND params[7];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (void *)&v->DataLastName_ID;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = (void *)&v->DataFirstName_ID;

    params[2].buffer_type = MYSQL_TYPE_LONG;
    params[2].buffer = (void *)&v->DataMiddleName_ID;

    params[3].buffer_type = MYSQL_TYPE_STRING;
    params[3].buffer = (void *)v->Suffix;
    params[3].buffer_length = sizeof(v->Suffix);
    params[3].length = &suffixLength;

    params[4].buffer_type = MYSQL_TYPE_STRING;
    params[4].buffer = (void *)v->DOB;
    params[4].buffer_length = sizeof(v->DOB);
    params[4].length = &dobLength;

    params[5].buffer_type = MYSQL_TYPE_LONG;
    params[5].buffer = (void *)&v->DataState_ID;

    params[6].buffer_type = MYSQL_TYPE_STRING;
    params[6].buffer = (void *)v->UniqStateVoterID;
    params[6].buffer_length = sizeof(v->UniqStateVoterID);
    params[6].length = &voterIdLength;

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
    unsigned long idLength = 0;
    MYSQL_BIND result;
    memset(&result, 0, sizeof(result));

    result.buffer_type = MYSQL_TYPE_LONG;
    result.buffer = &id;
    result.buffer_length = sizeof(id);
    result.length = &idLength;

    if (mysql_stmt_bind_result(stmt, &result) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Result setup failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == MYSQL_NO_DATA) {
        printf("Voter index not found in database.\n");
        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);
        return EXIT_SUCCESS;
    }

    if (fetchStatus != 0 && fetchStatus != MYSQL_DATA_TRUNCATED) {
        fprintf(stderr, "Fetch failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);
        return EXIT_FAILURE;
    }

    printf("Voter index ID: %" PRIu32 "\n", id);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    return EXIT_SUCCESS;
}