#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  int id;
  char name[255];
} Person;

typedef struct {
  size_t *buckets;
  size_t *next;
  size_t bucketCount;
  size_t collisions;
  const Person *Data;
} NameHashTable;

/* Function declarations */
MYSQL *DealDB(void);
int LoadDBintoMEM(MYSQL *, Person **, size_t *, char *);
int FindNameInDB(MYSQL *, const char *);
int FindNameInMEM(const Person *, size_t, const char *);
static double NowMilliseconds(void);
int CompareNameByName(const void *, const void *);
static uint64_t HashName(const char *);
int BuildNameHashTable(NameHashTable *, const Person *, size_t);
size_t FindNameInHASH(const NameHashTable *, const char *);
void PrintHashMatches(const NameHashTable *, size_t, const char *);
void FreeNameHashTable(NameHashTable *);

/* 
	DataCity
	DataMailingAddress
	DataMiddleName
	DataStreet
	DataStreetNonStdFormat
	
	DataAddress: Data Address
	Voters: Voters_ID, VotersIndexes_ID, DataHouse_ID, Voters_Gender, Voters_UniqStateVoterID, DataState_ID, Voters_RegParty, Voters_ReasonCode, Voters_Status, VotersMailingAddress_ID, Voters_IDRequired, Voters_IDMet, Voters_ApplyDate, Voters_RegSource, Voters_DateInactive, Voters_DatePurged, Voters_CountyVoterNumber, Voters_RMBActive, Voters_RecFirstSeen, Voters_RecLastSeen
	VotersIndexes: VotersIndexes_ID, DataLastName_ID, DataFirstName_ID, DataMiddleName_ID, VotersIndexes_Suffix, VotersIndexes_DOB, DataState_ID, VotersIndexes_UniqStateVoterID
*/

int main(void) {
  MYSQL *db = DealDB();
  if (db == NULL) { return EXIT_FAILURE; }

  Person *FirstName = NULL;
  Person *LastName = NULL;
  Person *MiddleName = NULL;
  
  size_t countFirstName = 0;
  size_t countLastName = 0;
  size_t countMiddleName = 0;
  
  int status = 0;

  NameHashTable FirstNamehashTable = {0};
  NameHashTable LastNamehashTable = {0};
  NameHashTable MiddleNamehashTable = {0};

	if (BuildNameHashTable(&FirstNamehashTable, FirstName, countFirstName) != EXIT_SUCCESS) { free(FirstName); mysql_close(db); return EXIT_FAILURE; }
	if (BuildNameHashTable(&LastNamehashTable, LastName, countLastName) != EXIT_SUCCESS) { free(LastName); mysql_close(db); return EXIT_FAILURE; }
	if (BuildNameHashTable(&MiddleNamehashTable, MiddleName, countMiddleName) != EXIT_SUCCESS) { free(MiddleName); mysql_close(db); return EXIT_FAILURE; }
  
  printf("\n--- Hash table statistics ---\n");
	printf("Firstname Rows:             %zu\n", countFirstName);
	printf("Buckets:          %zu\n", FirstNamehashTable.bucketCount);
	printf("Occupied buckets: %zu\n", countFirstName - FirstNamehashTable.collisions);
	printf("Collisions:       %zu\n", FirstNamehashTable.collisions);

	printf("Collision rate:   %.2f%%\n", countFirstName > 0 ? 
							100.0 * (double)FirstNamehashTable.collisions / (double)countFirstName : 0.0);
								
	printf("\nLastname Rows:             %zu\n", countLastName);
	printf("Buckets:          %zu\n", LastNamehashTable.bucketCount);
	printf("Occupied buckets: %zu\n", countLastName - LastNamehashTable.collisions);
	printf("Collisions:       %zu\n", LastNamehashTable.collisions);

	printf("Collision rate:   %.2f%%\n", countLastName > 0 ? 
							100.0 * (double)LastNamehashTable.collisions / (double)countLastName : 0.0);								

	printf("\nMiddlename Rows:             %zu\n", countMiddleName);
	printf("Buckets:          %zu\n", MiddleNamehashTable.bucketCount);
	printf("Occupied buckets: %zu\n", countLastName - MiddleNamehashTable.collisions);
	printf("Collisions:       %zu\n", MiddleNamehashTable.collisions);

	printf("Collision rate:   %.2f%%\n", countMiddleName > 0 ? 
							100.0 * (double)MiddleNamehashTable.collisions / (double)countMiddleName : 0.0);								

  printf("Press Ctrl+D on an empty line to quit.\n");

	char searchName[255];
	int exitStatus = EXIT_SUCCESS;

	while (1) {
    printf("\nEnter a first name: ");
    fflush(stdout);

    if (fgets(searchName, sizeof(searchName), stdin) == NULL) {
      if (ferror(stdin)) {
        fprintf(stderr, "Error reading input.\n");
        exitStatus = EXIT_FAILURE;
      }
      break;
    }

    /* Discard input that exceeds the buffer capacity. */
    if (strchr(searchName, '\n') == NULL) {
      int ch = getchar();

      if (ch != '\n' && ch != EOF) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
        	/* Discard the remainder of the line. */
        }
        fprintf(stderr, "Name too long. Maximum: %zu bytes.\n", sizeof(searchName) - 1);
        continue;
      }
    }

    searchName[strcspn(searchName, "\r\n")] = '\0';
    if (searchName[0] == '\0') { continue; }
    
    double start;
		double memoryMs;
		double databaseMs;

		printf("\n--- Memory search ---\n");

		start = NowMilliseconds();
		FindNameInMEM(FirstName, countFirstName, searchName);
		memoryMs = NowMilliseconds() - start;

		printf("\n--- Database search ---\n");

		start = NowMilliseconds();
		int dbStatus = FindNameInDB(db, searchName);
		databaseMs = NowMilliseconds() - start;
		
		printf("\n--- Hash table search ---\n");

		double hashStart = NowMilliseconds();
		size_t firstMatch = FindNameInHASH(&FirstNamehashTable, searchName);
		double hashMs = NowMilliseconds() - hashStart;

		/* Printing happens outside the timed lookup. */
		PrintHashMatches(&FirstNamehashTable, firstMatch, searchName);
		
		printf("\n");
		printf("Database search:   %.6f ms\n", databaseMs);
		printf("Memory search:     %.6f ms\n", memoryMs);
		printf("Hash lookup:       %.6f ms\n", hashMs);

		if (dbStatus != EXIT_SUCCESS) {
		    fprintf(stderr, "Database search failed.\n");
		    exitStatus = EXIT_FAILURE;
		    break;
		}

  }

	FreeNameHashTable(&FirstNamehashTable);
	free(FirstName);
	mysql_close(db);

  printf("\nGoodbye!\n");
  return exitStatus;
}

MYSQL *DealDB(void) {
  MYSQL *db = mysql_init(NULL);

  if (db == NULL) {
      fprintf(stderr, "MySQL initialization failed.\n");
      return NULL;
  }

  if (mysql_real_connect(db,
                         "192.168.199.18",
                         "usracct",
                         "usracct",
                         "RepMyBlock",
                         3306,
                         NULL,
                         0) == NULL) {
    fprintf(stderr, "Connection failed: %s\n",
            mysql_error(db));
    mysql_close(db);
    return NULL;
  }

  return db;
}

int LoadDBintoMEM(MYSQL *db, Person **FirstName, size_t *count, char *TableName)
{
  *FirstName = NULL;
  *count = 0;
  char query[2048];

	snprintf(query, sizeof(query),
	    "SELECT `%s_ID`, `%s_Text` "
	    "FROM `%s` "
	    "WHERE `%s_ID` IS NOT NULL "
	    "AND `%s_Text` IS NOT NULL",
	    TableName, TableName, TableName, TableName, TableName
	);

  if (mysql_query(db, query) != 0) {
    fprintf(stderr, "Query failed: %s\n", mysql_error(db));
    return EXIT_FAILURE;
  }

  MYSQL_RES *result = mysql_store_result(db);
  if (result == NULL) {
    fprintf(stderr, "Result failed: %s\n", mysql_error(db));
    return EXIT_FAILURE;
  }

  my_ulonglong rows = mysql_num_rows(result);

  if (rows == 0) {
    mysql_free_result(result);
    return EXIT_SUCCESS;
  }

  if (rows > (size_t)-1 / sizeof(Person)) {
    fprintf(stderr, "Too many rows to allocate.\n");
    mysql_free_result(result);
    return EXIT_FAILURE;
  }

  Person *array = malloc((size_t)rows * sizeof(*array));
  if (array == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    mysql_free_result(result);
    return EXIT_FAILURE;
  }

  MYSQL_ROW row;
  size_t i = 0;

  while ((row = mysql_fetch_row(result)) != NULL) {
    /* Assumes database IDs fit in a C int. */
    array[i].id = atoi(row[0]);
    snprintf(array[i].name, sizeof(array[i].name), "%s", row[1]);
    i++;
  }

  if (mysql_errno(db) != 0) {
    fprintf(stderr, "Read failed: %s\n", mysql_error(db));
    free(array);
    mysql_free_result(result);
    return EXIT_FAILURE;
  }

  mysql_free_result(result);
  *FirstName = array;
  *count = i;
  return EXIT_SUCCESS;
}

int FindNameInDB(MYSQL *db, const char *name) {
  if (name == NULL) {
	  return EXIT_FAILURE;
  }

  const char *query =
      "SELECT DataFirstName_ID, DataFirstName_Text "
      "FROM DataFirstName "
      "WHERE DataFirstName_Text = ? "
      "AND DataFirstName_ID IS NOT NULL";

  MYSQL_STMT *stmt = mysql_stmt_init(db);
  if (stmt == NULL) {
    fprintf(stderr, "Statement initialization failed: %s\n", mysql_error(db));
    return EXIT_FAILURE;
  }

  MYSQL_BIND input[1] = {0};
  unsigned long nameLength = (unsigned long)strlen(name);

  input[0].buffer_type = MYSQL_TYPE_STRING;
  input[0].buffer = (void *)name;
  input[0].buffer_length = nameLength;
  input[0].length = &nameLength;

  if (mysql_stmt_prepare(stmt, query,
                         (unsigned long)strlen(query)) != 0 ||
      mysql_stmt_bind_param(stmt, input) != 0 ||
      mysql_stmt_execute(stmt) != 0) {
      fprintf(stderr, "Query failed: %s\n",
              mysql_stmt_error(stmt));
      mysql_stmt_close(stmt);
      return EXIT_FAILURE;
  }

  int id = 0;
  char resultName[255] = {0};
  unsigned long resultNameLength = 0;

  MYSQL_BIND output[2] = {0};

  output[0].buffer_type = MYSQL_TYPE_LONG;
  output[0].buffer = &id;
  output[0].buffer_length = sizeof(id);

  output[1].buffer_type = MYSQL_TYPE_STRING;
  output[1].buffer = resultName;
  output[1].buffer_length = sizeof(resultName) - 1;
  output[1].length = &resultNameLength;

  if (mysql_stmt_bind_result(stmt, output) != 0 || mysql_stmt_store_result(stmt) != 0) {
    fprintf(stderr, "Result failed: %s\n", mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    return EXIT_FAILURE;
  }

  int found = 0;
  int fetchStatus;

  while ((fetchStatus = mysql_stmt_fetch(stmt)) == 0 || fetchStatus == MYSQL_DATA_TRUNCATED) {
    size_t length = resultNameLength;

    if (length >= sizeof(resultName)) {
      length = sizeof(resultName) - 1;
    }

    resultName[length] = '\0';
    printf("ID: %d | Name: %s\n", id, resultName);
    found = 1;
  }

  if (fetchStatus != MYSQL_NO_DATA) {
    fprintf(stderr, "Read failed: %s\n", mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    return EXIT_FAILURE;
  }

  if (!found) {
    printf("Name not found: %s\n", name);
  }

  mysql_stmt_close(stmt);
  return EXIT_SUCCESS;
}

int FindNameInMEM(const Person *FirstName, size_t count, const char *searchName) {
  size_t left = 0;
  size_t right = count;

  /* Find the first name that is >= searchName. */
  while (left < right) {
    size_t middle = left + (right - left) / 2;

    if (strcmp(FirstName[middle].name, searchName) < 0) {
      left = middle + 1;
    } else {
      right = middle;
    }
  }

  int found = 0;

  /* Print all matching names. */
  for (size_t i = left; i < count; i++) {
    if (strcmp(FirstName[i].name, searchName) != 0) {
      break;
    }

    printf("ID: %d | Name: %s\n",  FirstName[i].id, FirstName[i].name);
    found = 1;
  }

  if (!found) {
      printf("Name not found: %s\n", searchName);
  }

  return found;
}

static double NowMilliseconds(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    perror("clock_gettime");
    exit(EXIT_FAILURE);
  }
  return (double)ts.tv_sec * 1000.0  + (double)ts.tv_nsec / 1000000.0;
}

int CompareNameByName(const void *a, const void *b) {
  const Person *personA = a;
  const Person *personB = b;
  return strcmp(personA->name, personB->name);
}

/* FNV-1a string hash. */
static uint64_t HashName(const char *name) {
  uint64_t hash = UINT64_C(14695981039346656037);
  while (*name != '\0') {
    hash ^= (unsigned char)*name++;
    hash *= UINT64_C(1099511628211);
  }
  return hash;
}

int BuildNameHashTable(NameHashTable *table, const Person *data, size_t count) {
  *table = (NameHashTable){0};

  /* Use approximately two buckets per row. */
  if (count > (SIZE_MAX - 1) / 2) {
    fprintf(stderr, "Hash table is too large.\n");
    return EXIT_FAILURE;
  }

  size_t bucketCount = count * 2 + 1;

  if (bucketCount > SIZE_MAX / sizeof(size_t) ||
    count > SIZE_MAX / sizeof(size_t)) {
    fprintf(stderr, "Hash table allocation overflow.\n");
    return EXIT_FAILURE;
  }

  table->buckets = malloc(bucketCount * sizeof(*table->buckets));

  if (table->buckets == NULL) {
    fprintf(stderr, "Could not allocate hash buckets.\n");
    return EXIT_FAILURE;
  }

  if (count > 0) {
    table->next = malloc(count * sizeof(*table->next));
    if (table->next == NULL) {
      fprintf(stderr, "Could not allocate hash links.\n");
      FreeNameHashTable(table);
      return EXIT_FAILURE;
    }
  }

  table->bucketCount = bucketCount;
  table->Data = data;

  /* SIZE_MAX means no entry. */
  for (size_t i = 0; i < bucketCount; i++) {
    table->buckets[i] = SIZE_MAX;
  }

  /*
   * Each bucket holds a linked chain of array indices.
   * Different names can share a bucket.
   */
   
  for (size_t i = 0; i < count; i++) {
    size_t bucket = (size_t)(HashName(data[i].name) % bucketCount);
    /* Count insertion into an occupied bucket. */
    if (table->buckets[bucket] != SIZE_MAX) { table->collisions++; }
    table->next[i] = table->buckets[bucket];
    table->buckets[bucket] = i;
	}
   
  return EXIT_SUCCESS;
}

/* Return the first matching array index, or SIZE_MAX. */
size_t FindNameInHASH(const NameHashTable *table, const char *name) {
  if (table == NULL || table->bucketCount == 0 || name == NULL) {
    return SIZE_MAX;
  }

  size_t bucket = (size_t)(HashName(name) % table->bucketCount);

  for (size_t i = table->buckets[bucket]; i != SIZE_MAX; i = table->next[i]) {
    if (strcmp(table->Data[i].name, name) == 0) {
	    return i;
    }
  }
  return SIZE_MAX;
}

/* Print the first match and any duplicates in its bucket. */
void PrintHashMatches(const NameHashTable *table, size_t first, const char *name) {
  if (first == SIZE_MAX) {
    printf("Name not found: %s\n", name);
    return;
  }

  for (size_t i = first; i != SIZE_MAX; i = table->next[i]) {
    if (strcmp(table->Data[i].name, name) == 0) {
      printf("ID: %d | Name: %s\n", table->Data[i].id, table->Data[i].name);
    }
  }
}

void FreeNameHashTable(NameHashTable *table) {
  free(table->buckets);
  free(table->next);
  *table = (NameHashTable){0};
}