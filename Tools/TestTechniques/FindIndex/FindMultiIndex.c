#define _POSIX_C_SOURCE 200809L

#include "FindMultiIndex.h"
#include "DataModules.h"

static double NowMilliseconds(void);
static int ReadQueryLine(const char *, char *, size_t);
static int RunQueryLoops(MYSQL *, const QueryDefinition *, size_t);

static int ParseAddressQuery(char *, DataAddress *);

static int ParseCountyQuery(char *, void *);
static int ParseAddressRecord(char *, void *);
static int ParseDistrictQuery(char *, void *);
static int ParseDistrictCycleQuery(char *, void *);
static int ParseDistrictTemporalQuery(char *, void *);
static int ParseHouseQuery(char *, void *);
static int ParseVoterComplementInfoQuery(char *, void *);
static int ParseVoterIndexQuery(char *, void *);
static int ParseVoterQuery(char *, void *);

static uint64_t HashName(const char *);
static int IsValidISODate(const char *);

static void PrintModuleLoad(const char *label, size_t count, size_t recordSize, double elapsedMs) {
  uint64_t totalMs=(uint64_t)(elapsedMs+0.5);
  printf("Loaded and indexed %zu %s in %.3f ms (or %" PRIu64 " hours %" PRIu64
         " minutes and %" PRIu64 " seconds and %" PRIu64
         " ms) - %.2f MiB of source records (hash memory additional)\n",
         count, label, elapsedMs, totalMs/3600000, (totalMs/60000)%60,
         (totalMs/1000)%60, totalMs%1000,
         (double)count*recordSize/(1024.0*1024.0));
}

/* Include DataModules.h (or the eight module headers) above this main. */
int main(void) {

  MYSQL *db = DealDB();
  if (db == NULL) { return EXIT_FAILURE; }

  int exitStatus = EXIT_FAILURE;
  
  size_t countAddresses = 0, CountyCount = 0, DistrictCount = 0, DistrictCycleCount = 0, 
  				DistrictTemporalCount = 0, HouseCount = 0, VoterCount = 0, 
  				VoterComplementInfoCount = 0, VoterIndexCount = 0;

  AddressHashTable *addressTable = NULL;
  CountyHashTable *CountyTable = NULL;
  DistrictHashTable *DistrictTable = NULL;
  DistrictCycleHashTable *DistrictCycleTable = NULL;
  DistrictTemporalHashTable *DistrictTemporalTable = NULL;
  HouseHashTable *HouseTable = NULL;
  VoterHashTable *VoterTable = NULL;
  VoterComplementInfoHashTable *VoterComplementInfoTable = NULL;
  VoterIndexHashTable *VoterIndexTable = NULL;

	DataAddress *addresses = NULL;
  DataCounty *CountyRows = NULL;
  DataDistrict *DistrictRows = NULL;
  DataDistrictCycle *DistrictCycleRows = NULL;
  DataDistrictTemporal *DistrictTemporalRows = NULL;
  DataHouse *HouseRows = NULL;
  Voter *VoterRows = NULL;
  VoterComplementInfo *VoterComplementInfoRows = NULL;
  VoterIndex *VoterIndexRows = NULL;
  
  printf("Starting to load\n");
  double start = NowMilliseconds();
  if (LoadAddressesIntoMemory(db, &addresses, &countAddresses) != EXIT_SUCCESS) goto cleanup;
  addressTable = BuildAddressHashTable(addresses, countAddresses);
  if (!addressTable) { fprintf(stderr, "Address index failed.\n"); goto cleanup; }
  PrintModuleLoad("addresses", countAddresses, sizeof(*addresses), NowMilliseconds()-start);
  /* The address hash owns its keys and IDs. Release the original rows. */
  free(addresses); addresses = NULL;

  printf("Loading DataCounty...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadCountyIntoMemory(db, &CountyRows, &CountyCount) != EXIT_SUCCESS)  goto cleanup;
  CountyTable = BuildCountyHashTable(CountyRows, CountyCount);
  if (!CountyTable) { fprintf(stderr, "DataCounty index failed.\n"); goto cleanup; }
  PrintModuleLoad("DataCounty", CountyCount, sizeof(*CountyRows), NowMilliseconds()-start);
  free(CountyRows); CountyRows = NULL;

  printf("Loading DataDistrict...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadDistrictIntoMemory(db, &DistrictRows, &DistrictCount) != EXIT_SUCCESS) goto cleanup;
  DistrictTable = BuildDistrictHashTable(DistrictRows, DistrictCount);
  if (!DistrictTable) { fprintf(stderr, "DataDistrict index failed.\n"); goto cleanup; }
  PrintModuleLoad("DataDistrict", DistrictCount, sizeof(*DistrictRows), NowMilliseconds()-start);
  free(DistrictRows); DistrictRows = NULL;

  printf("Loading DataDistrictCycle...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadDistrictCycleIntoMemory(db, &DistrictCycleRows, &DistrictCycleCount) != EXIT_SUCCESS) goto cleanup;
  DistrictCycleTable = BuildDistrictCycleHashTable(DistrictCycleRows, DistrictCycleCount);
  if (!DistrictCycleTable) { fprintf(stderr, "DataDistrictCycle index failed.\n"); goto cleanup; }
  PrintModuleLoad("DataDistrictCycle", DistrictCycleCount, sizeof(*DistrictCycleRows), NowMilliseconds()-start);
  free(DistrictCycleRows); DistrictCycleRows = NULL;

  printf("Loading DataDistrictTemporal...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadDistrictTemporalIntoMemory(db, &DistrictTemporalRows, &DistrictTemporalCount) != EXIT_SUCCESS) goto cleanup;
  DistrictTemporalTable = BuildDistrictTemporalHashTable(DistrictTemporalRows, DistrictTemporalCount);
  if (!DistrictTemporalTable) { fprintf(stderr, "DataDistrictTemporal index failed.\n"); goto cleanup; }
  PrintModuleLoad("DataDistrictTemporal", DistrictTemporalCount, sizeof(*DistrictTemporalRows), NowMilliseconds()-start);
  free(DistrictTemporalRows); DistrictTemporalRows = NULL;

  printf("Loading DataHouse...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadHouseIntoMemory(db, &HouseRows, &HouseCount) != EXIT_SUCCESS) goto cleanup;
  HouseTable = BuildHouseHashTable(HouseRows, HouseCount);
  if (!HouseTable) { fprintf(stderr, "DataHouse index failed.\n"); goto cleanup; }
  PrintModuleLoad("DataHouse", HouseCount, sizeof(*HouseRows), NowMilliseconds()-start);
  free(HouseRows); HouseRows = NULL;

  printf("Loading Voter...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadVoterIntoMemory(db, &VoterRows, &VoterCount) != EXIT_SUCCESS) goto cleanup;
  VoterTable = BuildVoterHashTable(VoterRows, VoterCount);
  if (!VoterTable) { fprintf(stderr, "Voter index failed.\n"); goto cleanup; }
  PrintModuleLoad("Voter", VoterCount, sizeof(*VoterRows), NowMilliseconds()-start);
  free(VoterRows); VoterRows = NULL;

  printf("Loading VoterComplementInfo...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadVoterComplementInfoIntoMemory(db, &VoterComplementInfoRows, &VoterComplementInfoCount) != EXIT_SUCCESS) goto cleanup;
  VoterComplementInfoTable = BuildVoterComplementInfoHashTable(VoterComplementInfoRows, VoterComplementInfoCount);
  if (!VoterComplementInfoTable) { fprintf(stderr, "VoterComplementInfo index failed.\n"); goto cleanup; }
  PrintModuleLoad("VoterComplementInfo", VoterComplementInfoCount, sizeof(*VoterComplementInfoRows), NowMilliseconds()-start);
  free(VoterComplementInfoRows); VoterComplementInfoRows = NULL;

  printf("Loading VoterIndex...\n"); fflush(stdout);
  start = NowMilliseconds();
  if (LoadVoterIndexIntoMemory(db, &VoterIndexRows, &VoterIndexCount) != EXIT_SUCCESS) goto cleanup;
  VoterIndexTable = BuildVoterIndexHashTable(VoterIndexRows, VoterIndexCount);
  if (!VoterIndexTable) { fprintf(stderr, "VoterIndex index failed.\n"); goto cleanup; }
  PrintModuleLoad("VoterIndex", VoterIndexCount, sizeof(*VoterIndexRows), NowMilliseconds()-start);
  free(VoterIndexRows); VoterIndexRows = NULL;

	/*
  for (size_t i=0; i<3; ++i) {
    if (LoadDBintoMEM(db, &names[i], &nameCounts[i], nameSQLTables[i]) != EXIT_SUCCESS) goto cleanup;
    if (nameCounts[i]>1) qsort(names[i], nameCounts[i], sizeof(*names[i]), CompareNameByName);
    if (BuildNameHashTable(&nameTables[i], names[i], nameCounts[i]) != EXIT_SUCCESS) goto cleanup;
    printf("\n--- %s ---\nRows: %zu\nBuckets: %zu\nOccupied buckets: %zu\n"
           "Collisions: %zu\nCollision rate: %.2f%%\n",
           nameSQLTables[i], nameCounts[i], nameTables[i].bucketCount,
           nameCounts[i]-nameTables[i].collisions, nameTables[i].collisions,
           nameCounts[i] ? 100.0*(double)nameTables[i].collisions/nameCounts[i] : 0.0);
  }
  */
  /* Existing menu: first, middle, last names and addresses. */
  
  QueryDefinition queries[] = {
  	{ .key = 'A', .label = "Address search", .prompt = "Enter address fields: ", .hashTable = addressTable,
      .recordSize = sizeof(DataAddress), .parse = ParseAddressRecord, .findMemory = QueryAddressMemory,
      .findDatabase = QueryAddressDatabase, .print = NULL },
    { .key = 'C', .label = "County search", .prompt = "Enter county fields: ", .hashTable = CountyTable,
      .recordSize = sizeof(DataCounty), .parse = ParseCountyQuery, .findMemory = QueryCountyMemory,
      .findDatabase = QueryCountyDatabase, .print = NULL },
    { .key = 'D', .label = "District search", .prompt = "Enter district fields: ", .hashTable = DistrictTable,
      .recordSize = sizeof(DataDistrict), .parse = ParseDistrictQuery, .findMemory = QueryDistrictMemory,
      .findDatabase = QueryDistrictDatabase, .print = NULL },
    {.key = 'Y', .label = "District-cycle search", .prompt = "Enter district-cycle fields: ", .hashTable = DistrictCycleTable,
      .recordSize = sizeof(DataDistrictCycle), .parse = ParseDistrictCycleQuery, .findMemory = QueryDistrictCycleMemory,
      .findDatabase = QueryDistrictCycleDatabase, .print = NULL },
    { .key = 'T', .label = "District-temporal search", .prompt = "Enter district-temporal fields: ", .hashTable = DistrictTemporalTable,
      .recordSize = sizeof(DataDistrictTemporal), .parse = ParseDistrictTemporalQuery, .findMemory = QueryDistrictTemporalMemory,
      .findDatabase = QueryDistrictTemporalDatabase, .print = NULL },
    { .key = 'H', .label = "House search", .prompt = "Enter house fields: ", .hashTable = HouseTable, 
    	.recordSize = sizeof(DataHouse), .parse = ParseHouseQuery, .findMemory = QueryHouseMemory, 
      .findDatabase = QueryHouseDatabase, .print = NULL },
    { .key = 'V', .label = "Voter search", .prompt = "Enter voter fields: ", .hashTable = VoterTable,
      .recordSize = sizeof(Voter), .parse = ParseVoterQuery, .findMemory = QueryVoterMemory,
      .findDatabase = QueryVoterDatabase, .print = NULL },
    { .key = 'I', .label = "Voter-complement search", .prompt = "Enter voter-complement fields: ", .hashTable = VoterComplementInfoTable,
      .recordSize = sizeof(VoterComplementInfo), .parse = ParseVoterComplementInfoQuery, .findMemory = QueryVoterComplementInfoMemory,
      .findDatabase = QueryVoterComplementInfoDatabase, .print = NULL },
    { .key = 'X', .label = "Voter-index search", .prompt = "Enter voter-index fields: ", .hashTable = VoterIndexTable,
      .recordSize = sizeof(VoterIndex), .parse = ParseVoterIndexQuery, .findMemory = QueryVoterIndexMemory,
      .findDatabase = QueryVoterIndexDatabase, .print = NULL }
	};
	  
  int status = RunQueryLoops(db, queries, sizeof(queries) / sizeof(queries[0]));

cleanup:
    FreeAddressHashTable(addressTable);
    FreeCountyHashTable(CountyTable);
    FreeDistrictHashTable(DistrictTable);
    FreeDistrictCycleHashTable(DistrictCycleTable);
    FreeDistrictTemporalHashTable(DistrictTemporalTable);
    FreeHouseHashTable(HouseTable);
    FreeVoterHashTable(VoterTable);
    FreeVoterComplementInfoHashTable(VoterComplementInfoTable);
    FreeVoterIndexHashTable(VoterIndexTable);

    free(addresses);
    free(CountyRows);
    free(DistrictRows);
    free(DistrictCycleRows);
    free(DistrictTemporalRows);
    free(HouseRows);
    free(VoterRows);
    free(VoterComplementInfoRows);
    free(VoterIndexRows);

	/*
    for (size_t i=0; i<3; ++i) {
        FreeNameHashTable(&nameTables[i]);
        free(names[i]);
    }
    */
    
    mysql_close(db);
    return exitStatus;
}

MYSQL *DealDB(void) {
  MYSQL *db = mysql_init(NULL);

  if (db == NULL) {
    fprintf(stderr, "MySQL initialization failed.\n");
    return NULL;
  }

  if (mysql_options(db, MYSQL_READ_DEFAULT_GROUP, "RepMyBlockDev") != 0) {
    fprintf(stderr, "Could not configure login group: %s\n", mysql_error(db));
    mysql_close(db);
    return NULL;
  }

  /*
   * NULL credentials and port 0 allow the option-file
   * settings to supply host, user, password, and port.
   */
  if (mysql_real_connect(db, NULL, NULL, NULL, "RepMyBlock", 0, NULL, 0) == NULL) {
    fprintf(stderr, "Connection failed: %s\n", mysql_error(db));
    mysql_close(db);
    return NULL;
  }

  return db;
}

int LoadDBintoMEM(MYSQL *db, Person **FirstName, size_t *count, char *TableName) {
  *FirstName = NULL;
  *count = 0;
  char query[2048];

	snprintf(query, sizeof(query),
    "SELECT `%s_ID`, `%s_Text` FROM `%s` WHERE `%s_ID` IS NOT NULL AND `%s_Text` IS NOT NULL",
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

int FindNameInDB(MYSQL *db, const char *name, const char *tableName) {
  if (db == NULL || name == NULL || tableName == NULL) {
    return EXIT_FAILURE;
  }

  /* Only allow these three table names. */
  if (strcmp(tableName, "DataFirstName") != 0 && strcmp(tableName, "DataMiddleName") != 0 && strcmp(tableName, "DataLastName") != 0) {
    fprintf(stderr, "Invalid name table.\n");
    return EXIT_FAILURE;
  }

  char query[512];
  int length = snprintf(query, sizeof(query), 
									  		"SELECT `%s_ID`, `%s_Text` FROM `%s` WHERE `%s_Text` = ? AND `%s_ID` IS NOT NULL",
									      tableName, tableName, tableName, tableName, tableName);
  if (length < 0 || (size_t)length >= sizeof(query)) {
    fprintf(stderr, "Name query is too long.\n");
    return EXIT_FAILURE;
  }

  /* Keep the rest of your existing prepared-statement code here. */
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

  if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query)) != 0 ||  mysql_stmt_bind_param(stmt, input) != 0 ||  mysql_stmt_execute(stmt) != 0) {
	  fprintf(stderr, "Query failed: %s\n", mysql_stmt_error(stmt));
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

/*
 * Returns:
 *   1: line read
 *   0: end of input
 *  -1: input too long
 *  -2: input error
 */
static int ReadQueryLine(const char *prompt, char *buffer, size_t capacity) {
  printf("%s", prompt);
  fflush(stdout);

  if (fgets(buffer, capacity, stdin) == NULL) {
	  return ferror(stdin) ? -2 : 0;
  }

  if (strchr(buffer, '\n') == NULL) {
    int ch = getchar();

    if (ch != '\n' && ch != EOF) {
      while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remainder. */
      }

      if (ferror(stdin)) {
    		return -2;
      }

      fprintf(stderr, "Input too long.\n");
      return -1;
  	}

    if (ferror(stdin)) {
        return -2;
    }
	}

  buffer[strcspn(buffer, "\r\n")] = '\0';
  return 1;
}

static int RunQueryLoops(MYSQL *db, const QueryDefinition *queries, size_t queryCount) {
  char line[4096];

  while (1) {
    int status = ReadQueryLine(
        "\nQuery type: "
        "(F)irst, (M)iddle, (L)ast, "
        "(A)ddress, (C)ounty, (D)istrict, "
        "(Y)District cycle, (T)emporal, "
        "(H)ouse, (V)oter, (I)nfo, "
        "(X)ndex, (Q)uit: ",
        line,
        sizeof(line)
    );

    if (status == 0)  {return EXIT_SUCCESS; }
    if (status == -2) { return EXIT_FAILURE; }
    if (status == -1 || line[0] == '\0') { continue; }

    char choice = (char)toupper((unsigned char)line[0]);

    if (line[1] != '\0' && line[1] != '\n') {
      printf("Enter one query letter.\n");
      continue;
    }

    if (choice == 'Q') { return EXIT_SUCCESS; }

    const QueryDefinition *query = NULL;

    for (size_t i = 0; i < queryCount; ++i) {
      if (queries[i].key == choice) {
        query = &queries[i];
        break;
    	}
  	}

    if (query == NULL) {
      printf("Unknown query type.\n");
      continue;
    }

    printf("\n%s\n", query->label);
    printf("Empty line returns to the menu. Ctrl+D quits.\n");

    while (1) {
      status = ReadQueryLine(query->prompt, line, sizeof(line));

	    if (status == 0) { return EXIT_SUCCESS; }
	    if (status == -2) { return EXIT_FAILURE; }
	    if (status == -1) { continue;}
	    if (line[0] == '\0') { break; }
	    void *record = calloc(1, query->recordSize);

	    if (record == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
	    }

	    if (query->parse(line, record) != EXIT_SUCCESS) {
        free(record);
        continue;
	    }

	    double start = NowMilliseconds();
	    int memoryStatus = query->findMemory(query->hashTable, record);
	    double memoryMs = NowMilliseconds() - start;
	    printf("\n--- Memory hash search ---\n");

	    if (memoryStatus == 0) {
        printf("No memory match found.\n");
	    } else if (query->print != NULL) {
        query->print(record);
	    }

	    printf("Memory search: %.6f ms\n", memoryMs);
	    printf("\n--- Database search ---\n");
	    start = NowMilliseconds();
	    int databaseStatus = query->findDatabase(db, record);
	    double databaseMs = NowMilliseconds() - start;
      printf("Database search: %.6f ms\n", databaseMs);
      if (databaseStatus != EXIT_SUCCESS) {
        fprintf(stderr, "Database search failed.\n");
      }
      free(record);
    }
	}
}