#define _POSIX_C_SOURCE 200809L

#include "FindMultiIndex.h"
#include "DataAddress.h"

static double NowMilliseconds(void);
static uint64_t HashName(const char *);

int main(void) {

	AddressHashTable *addressTable = NULL;

  MYSQL *db = DealDB();
  if (db == NULL) { return EXIT_FAILURE; }

  DataAddress *addresses = NULL;
  size_t countAddresses = 0;

  Person *FirstName = NULL;
  Person *LastName = NULL;
  Person *MiddleName = NULL;

  size_t countFirstName = 0;
  size_t countLastName = 0;
  size_t countMiddleName = 0;

  NameHashTable FirstNamehashTable = {0};
  NameHashTable LastNamehashTable = {0};
  NameHashTable MiddleNamehashTable = {0};

  int exitStatus = EXIT_FAILURE;
  
  printf("Starting to load\n");

  double addressStart = NowMilliseconds();
  if (LoadAddressesIntoMemory(db, &addresses, &countAddresses) != EXIT_SUCCESS) goto cleanup;
  	
 	addressTable = BuildAddressHashTable(addresses, countAddresses);

	if (addressTable == NULL) {
    fprintf(stderr, "Could not build address hash table.\n");
    goto cleanup;
	}
        
	double elapsedMs = NowMilliseconds() - addressStart;

	/* Round to the nearest millisecond for the readable duration. */
	uint64_t totalMs = (uint64_t)(elapsedMs + 0.5);
  uint64_t hours        = totalMs / 3600000;
	uint64_t minutes      = (totalMs / 60000) % 60;
	uint64_t seconds      = (totalMs / 1000) % 60;
	uint64_t milliseconds = totalMs % 1000;
	
	double memoryMiB = (double)countAddresses * sizeof(*addresses) / (1024.0 * 1024.0);

	printf(
			"Loaded %zu addresses into memory in %.3f ms (or %" PRIu64 " hours %" PRIu64 " minutes "
	    "and %" PRIu64 " seconds and %" PRIu64 " ms) - %.2f MiB of records\n",
	    countAddresses, elapsedMs, hours, minutes,seconds, milliseconds, memoryMiB
	);
	        
  /* Load the three name tables. */
  if (LoadDBintoMEM(db, &FirstName, &countFirstName, "DataFirstName") != EXIT_SUCCESS) { goto cleanup; }
  if (LoadDBintoMEM(db, &LastName, &countLastName, "DataLastName") != EXIT_SUCCESS) { goto cleanup; }
  if (LoadDBintoMEM(db, &MiddleName, &countMiddleName, "DataMiddleName") != EXIT_SUCCESS) { goto cleanup; }

  /* Sort before building indexes. Binary search requires this. */
  if (countFirstName > 1) { qsort(FirstName, countFirstName, sizeof(*FirstName), CompareNameByName); }
  if (countLastName > 1) { qsort(LastName, countLastName, sizeof(*LastName), CompareNameByName); }
  if (countMiddleName > 1) { qsort(MiddleName, countMiddleName, sizeof(*MiddleName), CompareNameByName); }

  /* Build hash tables after sorting. */
  if (BuildNameHashTable(&FirstNamehashTable, FirstName, countFirstName) != EXIT_SUCCESS) { goto cleanup; }
  if (BuildNameHashTable(&LastNamehashTable, LastName, countLastName) != EXIT_SUCCESS) { goto cleanup; }
  if (BuildNameHashTable(&MiddleNamehashTable, MiddleName, countMiddleName) != EXIT_SUCCESS) { goto cleanup; }

  /* Display statistics for each hash table. */
  const char *labels[] = { "First names", "Last names", "Middle names" };
  const NameHashTable *tables[] = { &FirstNamehashTable, &LastNamehashTable, &MiddleNamehashTable };
  size_t counts[] = { countFirstName, countLastName, countMiddleName };

  for (size_t i = 0; i < 3; i++) {
    printf("\n--- %s ---\n", labels[i]);
    printf("Rows:             %zu\n", counts[i]);
    printf("Buckets:          %zu\n", tables[i]->bucketCount);
    printf("Occupied buckets: %zu\n", counts[i] - tables[i]->collisions);
    printf("Collisions:       %zu\n", tables[i]->collisions);
    printf("Collision rate:   %.2f%%\n", counts[i] > 0 ? 
    																100.0 * (double)tables[i]->collisions / (double)counts[i] : 0.0);
	}	
  exitStatus = EXIT_SUCCESS;

  printf("\nPress Ctrl+D on an empty line to quit.\n");
  exitStatus = RunQueryLoops(db, &FirstNamehashTable, &MiddleNamehashTable, &LastNamehashTable, addressTable);
	
cleanup:
	
	FreeAddressHashTable(addressTable);
  free(addresses);
  FreeNameHashTable(&FirstNamehashTable);
  FreeNameHashTable(&LastNamehashTable);
  FreeNameHashTable(&MiddleNamehashTable);
	
  free(FirstName);
  free(LastName);
  free(MiddleName);

  mysql_close(db);
  return exitStatus;
}


MYSQL *DealDB(void)
{
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

int FindNameInDB(MYSQL *db, const char *name, const char *tableName)
{
    if (db == NULL || name == NULL || tableName == NULL) {
        return EXIT_FAILURE;
    }

    /* Only allow these three table names. */
    if (strcmp(tableName, "DataFirstName") != 0 &&
        strcmp(tableName, "DataMiddleName") != 0 &&
        strcmp(tableName, "DataLastName") != 0) {
        fprintf(stderr, "Invalid name table.\n");
        return EXIT_FAILURE;
    }

    char query[512];

    int length = snprintf(
        query, sizeof(query),
        "SELECT `%s_ID`, `%s_Text` "
        "FROM `%s` "
        "WHERE `%s_Text` = ? "
        "AND `%s_ID` IS NOT NULL",
        tableName, tableName, tableName, tableName, tableName
    );

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

static int ParseAddressQuery(char *line, DataAddress *address)
{
    char *fields[11];
    size_t fieldCount = 1;

    fields[0] = line;

    /*
     * Unlike strtok(), this preserves empty fields between commas.
     */
    for (char *p = line; *p != '\0'; ++p) {
        if (*p == ',') {
            if (fieldCount == 11) {
                fprintf(stderr, "Too many fields: expected 11.\n");
                return EXIT_FAILURE;
            }

            *p = '\0';
            fields[fieldCount++] = p + 1;
        }
    }

    if (fieldCount != 11) {
        fprintf(stderr, "Expected 11 fields; received %zu.\n",
                fieldCount);
        return EXIT_FAILURE;
    }

    *address = (DataAddress){0};

    /* Field zero is database column one: HouseNumber. */
    for (size_t i = 0; i < 11; ++i) {
        if (fields[i][0] == '\0') {
            address->nullFields |= (uint16_t)(1u << (i + 1));
        }
    }

    const size_t textIndexes[] = {0, 1, 2, 4, 7, 8};

    char *destinations[] = {
        address->HouseNumber,
        address->FracAddress,
        address->PreStreet,
        address->PostStreet,
        address->zipcode,
        address->zip4
    };

    const size_t capacities[] = {
        sizeof(address->HouseNumber),
        sizeof(address->FracAddress),
        sizeof(address->PreStreet),
        sizeof(address->PostStreet),
        sizeof(address->zipcode),
        sizeof(address->zip4)
    };

    for (size_t i = 0; i < 6; ++i) {
        const char *value = fields[textIndexes[i]];
        size_t length = strlen(value);

        if (length >= capacities[i]) {
            fprintf(stderr, "Address field %zu is too long.\n",
                    textIndexes[i] + 1);
            return EXIT_FAILURE;
        }

        memcpy(destinations[i], value, length + 1);
    }

    const size_t numericIndexes[] = {3, 5, 6, 9};

    uint32_t *numericDestinations[] = {
        &address->DataStreet_ID,
        &address->DataCity_ID,
        &address->DataCounty_ID,
        &address->Cordinate_ID
    };

    for (size_t i = 0; i < 4; ++i) {
        const char *value = fields[numericIndexes[i]];

        if (*value == '\0') {
            continue; /* SQL NULL */
        }

        if (strspn(value, "0123456789") != strlen(value)) {
            fprintf(stderr, "Field %zu must be an unsigned integer.\n",
                    numericIndexes[i] + 1);
            return EXIT_FAILURE;
        }

        char *end;
        errno = 0;
        uintmax_t number = strtoumax(value, &end, 10);

        if (errno != 0 || *end != '\0' || number > UINT32_MAX) {
            fprintf(stderr, "Field %zu is outside the uint32_t range.\n",
                    numericIndexes[i] + 1);
            return EXIT_FAILURE;
        }

        *numericDestinations[i] = (uint32_t)number;
    }

    if (fields[10][0] != '\0') {
        const char *value = fields[10];
        const char *digits = (*value == '-') ? value + 1 : value;

        if (*digits == '\0' ||
            strspn(digits, "0123456789") != strlen(digits)) {
            fprintf(stderr, "OSM ID must be a signed integer.\n");
            return EXIT_FAILURE;
        }

        char *end;
        errno = 0;
        intmax_t number = strtoimax(value, &end, 10);

        if (errno != 0 || *end != '\0' ||
            number < INT64_MIN || number > INT64_MAX) {
            fprintf(stderr, "OSM ID is outside the int64_t range.\n");
            return EXIT_FAILURE;
        }

        address->PG_OSM_osmid = (int64_t)number;
    }

    return EXIT_SUCCESS;
}

static int RunQueryLoops(MYSQL *db, const NameHashTable *firstNames, const NameHashTable *middleNames,
											    const NameHashTable *lastNames, const AddressHashTable *addresses) {
	char line[4096];
	const NameHashTable *nameTable = NULL;
	const char *prompt = NULL;
	const char *tableName = NULL;


	while (1) {
	    int inputStatus = ReadQueryLine(
	        "\nQuery type: (F)irst Name, (M)iddle Name, "
	        "(L)ast Name, (A)ddress, (Q)uit: ",
	        line, sizeof(line));

	    if (inputStatus == 0) {
	        return EXIT_SUCCESS;
	    }
	    if (inputStatus == -2) {
	        return EXIT_FAILURE;
	    }
	    if (inputStatus == -1 || line[0] == '\0') {
	        continue;
	    }

	    int choice = toupper((unsigned char)line[0]);

	    if (line[1] != '\0' ||
	        (choice != 'F' && choice != 'M' &&
	         choice != 'L' && choice != 'A' && choice != 'Q')) {
	        printf("Enter F, M, L, A, or Q.\n");
	        continue;
	    }

	    if (choice == 'Q') {
	        return EXIT_SUCCESS;
	    }

	 
	
        switch (choice) {
          case 'F':
				    nameTable = firstNames;
				    tableName = "DataFirstName";
				    prompt = "Enter the FIRST name: ";
				    break;

           case 'M':
					    nameTable = middleNames;
					    tableName = "DataMiddleName";
					    prompt = "Enter the MIDDLE name: ";
					    break;

					case 'L':
					    nameTable = lastNames;
					    tableName = "DataLastName";
					    prompt = "Enter the LAST name: ";
    			break;

            case 'A':
                if (addresses == NULL) {
                    printf("The address hash table is not available.\n");
                    continue;
                }

                prompt = "Enter the address: ";
                printf(
                    "\nField order:\n"
                    "HouseNumber,FracAddress,PreStreet,DataStreet_ID,"
                    "PostStreet,DataCity_ID,DataCounty_ID,zipcode,zip4,"
                    "Cordinate_ID,PG_OSM_osmid\n");
                break;
        }

        printf("Empty line: return to menu. Ctrl+D: quit.\n");

        while (1) {
            inputStatus = ReadQueryLine(prompt, line, sizeof(line));

            if (inputStatus == 0) {
                return EXIT_SUCCESS;
            }
            if (inputStatus == -2) {
                return EXIT_FAILURE;
            }
            if (inputStatus == -1) {
                continue;
            }
            if (line[0] == '\0') {
                break;
            }

            if (choice != 'A') {
						    printf("\n--- Memory hash search ---\n");

						    double start = NowMilliseconds();
						    size_t match = FindNameInHASH(nameTable, line);
						    double hashMs = NowMilliseconds() - start;

						    PrintHashMatches(nameTable, match, line);

						    printf("\n--- Database search ---\n");

						    start = NowMilliseconds();
						    int dbStatus = FindNameInDB(db, line, tableName);
						    double databaseMs = NowMilliseconds() - start;

						    printf("\nHash lookup:     %.6f ms\n", hashMs);
						    printf("Database search: %.6f ms\n", databaseMs);

						    if (dbStatus != EXIT_SUCCESS) {
						        fprintf(stderr, "Database name search failed.\n");
						    }

						    continue;
						}

            DataAddress search;

            if (ParseAddressQuery(line, &search) != EXIT_SUCCESS) {
                continue;
            }

            double start = NowMilliseconds();
            AddressMatch match = FindAddressID(addresses, &search);
            double elapsed = NowMilliseconds() - start;

            switch (match.status) {
                case ADDRESS_FOUND:
                case ADDRESS_DUPLICATE:
                    if (match.status == ADDRESS_DUPLICATE) {
                        printf("Duplicate address: %zu matches\n",
                               match.count);
                    }

                    for (size_t i = 0; i < match.count; ++i) {
                        printf("Address ID: %" PRIu32 "\n",
                               match.IDs[i]);
                    }
                    break;

                case ADDRESS_NOT_FOUND:
                    printf("Address not found.\n");
                    break;

                case ADDRESS_ERROR:
                default:
                    fprintf(stderr, "Address lookup failed.\n");
                    break;
            }

            FreeAddressMatch(&match);
            printf("Address lookup: %.6f ms\n", elapsed);
            printf("\n--- Database address search ---\n");

						double databaseStart = NowMilliseconds();
						int databaseStatus = FindAddressInDB(db, &search);
						double databaseMs = NowMilliseconds() - databaseStart;

						printf("Database address search: %.6f ms\n", databaseMs);

						if (databaseStatus != EXIT_SUCCESS) {
						    fprintf(stderr, "Database address search failed.\n");
						}
        }
    }
}