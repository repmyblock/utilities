#define _POSIX_C_SOURCE 200809L

#include "CommonFiles.h"

static int TestRandomAddresses(MYSQL *, const AddressHashTable *);

int main(int argc, char **argv) {
	
	size_t limit = 0;  /* Default: load everything */
	
	if (argc > 1) {
		char *end = NULL;
		errno = 0;
		uintmax_t value = strtoumax(argv[1], &end, 10);

		if (errno != 0 || end == argv[1] || *end != '\0' || value > SIZE_MAX) {
		  fprintf(stderr, "Invalid row limit: %s\n", argv[1]);
		  return EXIT_FAILURE;
		}

		limit = (size_t)value;
	}

  MYSQL *db = DealDB();
  if (db == NULL) return EXIT_FAILURE;

  DataAddress *addresses = NULL;
  size_t addressCount = 0;
  AddressHashTable *addressTable = NULL;
  int status = EXIT_FAILURE;
  char line[4096];

	double startMs = NowMilliseconds();

  if (LoadAddressesIntoMemory(db, &addresses, &addressCount, limit) != EXIT_SUCCESS) {
    fprintf(stderr, "Could not load DataAddress rows.\n");
    goto cleanup;
  }

	// Load the HashTable
  addressTable = BuildAddressHashTable(addresses, addressCount);
  if (addressTable == NULL) {
    fprintf(stderr, "Could not build the DataAddress hash table.\n");
    goto cleanup;
  }
  
  PrintModuleLoad("DataAddress", addressCount, sizeof(*addresses), NowMilliseconds()-startMs);
  TestRandomAddresses(db, addressTable);
  printf("DataAddress module test passed.\n\n");
  
  printf("Enter 9 comma-separated fields in this order:\n");
  printf("HouseNumber, FracAddress, PreStreet, DataStreet_ID, PostStreet, DataCity_ID, DataCounty_ID, zipcode, zip4\n");
  printf("Press Enter on an empty line to quit.\n");

  while (1) {
    printf("\nAddress: ");
    fflush(stdout);

    if (fgets(line, sizeof(line), stdin) == NULL) {
      if (ferror(stdin)) {
        perror("fgets");
        goto cleanup;
      }
      break;  /* End of input */
    }

    line[strcspn(line, "\r\n")] = '\0';
    if (line[0] == '\0') break;

    DataAddress query = {0};
    if (ParseAddressQuery(line, &query) != EXIT_SUCCESS) continue;

    printf("\n--- In-memory hash lookup ---\n");
    QueryAddressMemory(addressTable, &query);

    printf("\n--- Database lookup ---\n");
    if (QueryAddressDatabase(db, &query) != EXIT_SUCCESS) 
    	fprintf(stderr, "Database address lookup failed.\n");
  }

  status = EXIT_SUCCESS;

cleanup:
  FreeAddressHashTable(addressTable);
  free(addresses);
  mysql_close(db);
  return status;
}

static int TestRandomAddresses(MYSQL *db, const AddressHashTable *addressTable) {
	const char *query =
				"SELECT CONCAT("
				"COALESCE(DataAddress_HouseNumber, ''), ',',"
				"COALESCE(DataAddress_FracAddress, ''), ',',"
				"COALESCE(DataAddress_PreStreet, ''), ',',"
				"COALESCE(CAST(DataStreet_ID AS CHAR), ''), ',',"
				"COALESCE(DataAddress_PostStreet, ''), ',',"
				"COALESCE(CAST(DataCity_ID AS CHAR), ''), ',',"
				"COALESCE(CAST(DataCounty_ID AS CHAR), ''), ',',"
				"COALESCE(DataAddress_zipcode, ''), ',',"
				"COALESCE(DataAddress_zip4, '')"
				") AS AddressInput "
				"FROM RepMyBlock.DataAddress "
				"ORDER BY RAND() "
				"LIMIT 10";

	if (mysql_query(db, query) != 0) {
		fprintf(stderr, "Random address query failed: %s\n", mysql_error(db));
		return EXIT_FAILURE;
	}

	MYSQL_RES *result = mysql_store_result(db);
	if (result == NULL) {
		fprintf(stderr, "Could not retrieve random addresses: %s\n", mysql_error(db));
		return EXIT_FAILURE;
	}

	MYSQL_ROW row;
	size_t testNumber = 0;
	int status = EXIT_SUCCESS;

	while ((row = mysql_fetch_row(result)) != NULL) {
		if (row[0] == NULL) {
		  fprintf(stderr, "Random address query returned NULL.\n");
		  status = EXIT_FAILURE;
		  continue;
		}

		++testNumber;
		printf("\n--- Random address test %zu ---\n%s\n",  testNumber, row[0]);

		char input[4096];
		int n = snprintf(input, sizeof(input), "%s", row[0]);
		if (n < 0 || (size_t)n >= sizeof(input)) {
		  fprintf(stderr, "Random address %zu is too long.\n", testNumber);
		  status = EXIT_FAILURE;
		  continue;
		}

		DataAddress address = {0};
		if (ParseAddressQuery(input, &address) != EXIT_SUCCESS) {
		  fprintf(stderr, "Could not parse random address %zu.\n", testNumber);
		  status = EXIT_FAILURE;
		  continue;
		}

		printf("In-memory lookup: ");
		QueryAddressMemory(addressTable, &address);

		printf("Database lookup: ");
		if (QueryAddressDatabase(db, &address) != EXIT_SUCCESS) {
		  fprintf(stderr, "Database lookup failed for test %zu.\n", testNumber);
		  status = EXIT_FAILURE;
		}
	}

	if (mysql_errno(db) != 0) {
		fprintf(stderr, "Reading random addresses failed: %s\n", mysql_error(db));
		status = EXIT_FAILURE;
	}

	mysql_free_result(result);
	return status;
}