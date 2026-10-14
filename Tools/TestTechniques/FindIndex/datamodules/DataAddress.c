#include "DataAddress.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

/* Private: normalized UTF-8 strings owned by this key. */
typedef struct {
    uint16_t nullFields;
    char *HouseNumber;
    char *FracAddress;
    char *PreStreet;
    char *PostStreet;
    char *zipcode;
    char *zip4;

    uint32_t DataStreet_ID;
    uint32_t DataCity_ID;
    uint32_t DataCounty_ID;
} AddressKey;

typedef struct {
    AddressKey key;
    uint64_t hash;
    uint32_t ID;
    size_t next;
} AddressHashEntry;

/* Definition of the opaque type declared in DataAddress.h. */
struct AddressHashTable {
    size_t *buckets;
    AddressHashEntry *entries;
    size_t bucketCount;
    size_t count;
};

void FreeAddressMatch(AddressMatch *match) {
  if (match == NULL) { return; }
  free(match->IDs);

  *match = (AddressMatch) {
    .status = ADDRESS_NOT_FOUND,
    .IDs = NULL,
    .count = 0
  };
}

/* Parse database integers without truncation or signed wraparound. */
static int ReadUnsignedAddressID(const char *text, uint32_t *value) {
  char *end;
  if (!text || !*text || *text == '-') return 0;
  errno = 0;
  uintmax_t number = strtoumax(text, &end, 10);
  if (errno || *end || number > UINT32_MAX) return 0;
  *value = (uint32_t)number;
  return 1;
}

static int DecodeAddress(MYSQL_ROW row, const unsigned long *lengths, DataAddress *address) {
  *address = (DataAddress){0};
  if (!row[0]) return 0;
  for (size_t i = 1; i < 12; ++i)
      if (!row[i]) address->nullFields |= (uint16_t)(1u << i);

  const size_t numericColumns[] = {0, 4, 6, 7, 10};
  uint32_t *numericValues[] = {&address->ID, &address->DataStreet_ID,
      &address->DataCity_ID, &address->DataCounty_ID, &address->Cordinate_ID};
  for (size_t i = 0; i < 5; ++i) {
      size_t col = numericColumns[i];
      if (row[col] && !ReadUnsignedAddressID(row[col], numericValues[i]))
          return 0;
  }
  if (row[11]) {
      char *end;
      errno = 0;
      intmax_t number = strtoimax(row[11], &end, 10);
      if (errno || end == row[11] || *end ||
          number < INT64_MIN || number > INT64_MAX) return 0;
      address->PG_OSM_osmid = (int64_t)number;
  }

  const size_t textColumns[] = {1, 2, 3, 5, 8, 9};
  char *textValues[] = {address->HouseNumber, address->FracAddress,
      address->PreStreet, address->PostStreet, address->zipcode, address->zip4};
  const size_t capacities[] = {sizeof(address->HouseNumber),
      sizeof(address->FracAddress), sizeof(address->PreStreet),
      sizeof(address->PostStreet), sizeof(address->zipcode), sizeof(address->zip4)};
  for (size_t i = 0; i < 6; ++i) {
      size_t col = textColumns[i];
      if (!row[col]) continue;
      /* Reject truncation and embedded NULs in C strings. */
      if (lengths[col] >= capacities[i] ||
          memchr(row[col], '\0', lengths[col])) return 0;
      memcpy(textValues[i], row[col], lengths[col]);
      textValues[i][lengths[col]] = '\0';
  }
  return 1;
}

int LoadAddressesIntoMemory(MYSQL *db, DataAddress **addresses, size_t *count)
{
    if (!addresses || !count) return EXIT_FAILURE;
    *addresses = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;

    if (mysql_set_character_set(db, "utf8mb4") != 0) {
        fprintf(stderr, "Address charset failed: %s\n", mysql_error(db));
        return EXIT_FAILURE;
    }
    const char *query =
        "SELECT DataAddress_ID, DataAddress_HouseNumber, "
        "DataAddress_FracAddress, DataAddress_PreStreet, DataStreet_ID, "
        "DataAddress_PostStreet, DataCity_ID, DataCounty_ID, "
        "DataAddress_zipcode, DataAddress_zip4, Cordinate_ID, PG_OSM_osmid "
        "FROM DataAddress";
    if (mysql_query(db, query) != 0) {
        fprintf(stderr, "Address query failed: %s\n", mysql_error(db));
        return EXIT_FAILURE;
    }
    MYSQL_RES *result = mysql_use_result(db);
    if (!result) {
        fprintf(stderr, "Address result failed: %s\n", mysql_error(db));
        return EXIT_FAILURE;
    }

    DataAddress *array = NULL;
    size_t used = 0, capacity = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL) {
        if (used == capacity) {
            size_t maximum = SIZE_MAX / sizeof(*array);
            if (capacity == maximum) goto fail;
            size_t next = capacity ? capacity + capacity / 2 : 1024;
            if (next < capacity || next > maximum) next = maximum;
            DataAddress *grown = realloc(array, next * sizeof(*array));
            if (!grown) {
                fprintf(stderr, "Address allocation failed after %zu rows.\n", used);
                goto fail;
            }
            array = grown;
            capacity = next;
        }
        unsigned long *lengths = mysql_fetch_lengths(result);
        if (!lengths || !DecodeAddress(row, lengths, &array[used])) {
            fprintf(stderr, "Invalid or oversized address at result row %zu.\n", used + 1);
            goto fail;
        }
        ++used;
    }
    if (mysql_errno(db)) {
        fprintf(stderr, "Address read failed: %s\n", mysql_error(db));
        goto fail;
    }
    mysql_free_result(result);
    if (used && used < capacity) {
        DataAddress *smaller = realloc(array, used * sizeof(*array));
        if (smaller) array = smaller;
    }
    *addresses = array;
    *count = used;
    return EXIT_SUCCESS;

fail:
    /* Freeing an unbuffered result discards its unread rows. */
    mysql_free_result(result);
    free(array);
    return EXIT_FAILURE;
}

/* Unicode canonical normalization and default (locale-independent) folding. */
#include <unicode/ustring.h>
#include <unicode/unorm2.h>
#include <unicode/uchar.h>
#include <unicode/utf16.h>

static char *NormalizeAddressText(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    if (!end) return NULL;
    UChar decoded[4096], decomposed[4096], folded[4096], normalized[4096];
    int32_t length = 0;
    UErrorCode error = U_ZERO_ERROR;
    u_strFromUTF8(decoded, 4096, &length, text, (int32_t)(end - text), &error);
    if (U_FAILURE(error)) return NULL;

    int32_t first = 0, last = length;
    while (first < last) {
        int32_t next = first;
        UChar32 cp;
        U16_NEXT(decoded, next, last, cp);
        if (!u_isUWhiteSpace(cp)) break;
        first = next;
    }
    while (last > first) {
        int32_t previous = last;
        UChar32 cp;
        U16_PREV(decoded, first, previous, cp);
        if (!u_isUWhiteSpace(cp)) break;
        last = previous;
    }
    const UNormalizer2 *nfd = unorm2_getNFDInstance(&error);
    const UNormalizer2 *nfc = unorm2_getNFCInstance(&error);
    if (U_FAILURE(error)) return NULL;
    length = unorm2_normalize(nfd, decoded + first, last - first,
                             decomposed, 4096, &error);
    if (U_FAILURE(error)) return NULL;
    length = u_strFoldCase(folded, 4096, decomposed, length,
                          U_FOLD_CASE_DEFAULT, &error);
    if (U_FAILURE(error)) return NULL;
    length = unorm2_normalize(nfc, folded, length, normalized, 4096, &error);
    if (U_FAILURE(error)) return NULL;

    int32_t bytes = 0;
    u_strToUTF8(NULL, 0, &bytes, normalized, length, &error);
    if (error != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(error)) return NULL;
    error = U_ZERO_ERROR;
    char *result = malloc((size_t)bytes + 1);
    if (!result) return NULL;
    u_strToUTF8(result, bytes + 1, NULL, normalized, length, &error);
    if (U_FAILURE(error)) { free(result); return NULL; }
    result[bytes] = '\0';
    return result;
}

static void FreeAddressKey(AddressKey *key)
{
    free(key->HouseNumber); free(key->FracAddress); free(key->PreStreet);
    free(key->PostStreet); free(key->zipcode); free(key->zip4);
    *key = (AddressKey){0};
}

static int MakeAddressKey(const DataAddress *address, AddressKey *key)
{
    *key = (AddressKey){0};
    /* Only the nine matching columns participate in NULL comparison. */
    key->nullFields = address->nullFields & 0x03feu;
    key->DataStreet_ID = (key->nullFields & (1u << 4)) ? 0 : address->DataStreet_ID;
    key->DataCity_ID = (key->nullFields & (1u << 6)) ? 0 : address->DataCity_ID;
    key->DataCounty_ID = (key->nullFields & (1u << 7)) ? 0 : address->DataCounty_ID;
    const char *sources[] = {address->HouseNumber, address->FracAddress,
        address->PreStreet, address->PostStreet, address->zipcode, address->zip4};
    const size_t capacities[] = {sizeof(address->HouseNumber), sizeof(address->FracAddress),
        sizeof(address->PreStreet), sizeof(address->PostStreet),
        sizeof(address->zipcode), sizeof(address->zip4)};
    char **destinations[] = {&key->HouseNumber, &key->FracAddress, &key->PreStreet,
        &key->PostStreet, &key->zipcode, &key->zip4};
    const unsigned columns[] = {1, 2, 3, 5, 8, 9};
    for (size_t i = 0; i < 6; ++i) {
        if (key->nullFields & (1u << columns[i])) continue;
        *destinations[i] = NormalizeAddressText(sources[i], capacities[i]);
        if (!*destinations[i]) { FreeAddressKey(key); return 0; }
    }
    return 1;
}

static uint64_t HashAddressBytes(uint64_t hash, const void *data, size_t length)
{
    const unsigned char *bytes = data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static uint64_t HashAddressKey(const AddressKey *key)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    hash = HashAddressBytes(hash, &key->nullFields, sizeof(key->nullFields));
    hash = HashAddressBytes(hash, &key->DataStreet_ID, sizeof(key->DataStreet_ID));
    hash = HashAddressBytes(hash, &key->DataCity_ID, sizeof(key->DataCity_ID));
    hash = HashAddressBytes(hash, &key->DataCounty_ID, sizeof(key->DataCounty_ID));
    const char *texts[] = {key->HouseNumber, key->FracAddress, key->PreStreet,
        key->PostStreet, key->zipcode, key->zip4};
    for (size_t i = 0; i < 6; ++i) {
        /* Include terminators to preserve field boundaries. */
        const char *text = texts[i] ? texts[i] : "";
        hash = HashAddressBytes(hash, text, strlen(text) + 1);
    }
    return hash;
}

static int AddressKeysEqual(const AddressKey *a, const AddressKey *b)
{
    if (a->nullFields != b->nullFields || a->DataStreet_ID != b->DataStreet_ID ||
        a->DataCity_ID != b->DataCity_ID || a->DataCounty_ID != b->DataCounty_ID)
        return 0;
    const char *left[] = {a->HouseNumber, a->FracAddress, a->PreStreet,
        a->PostStreet, a->zipcode, a->zip4};
    const char *right[] = {b->HouseNumber, b->FracAddress, b->PreStreet,
        b->PostStreet, b->zipcode, b->zip4};
    for (size_t i = 0; i < 6; ++i) {
        if (!left[i] || !right[i]) {
            if (left[i] != right[i]) return 0;
        } else if (strcmp(left[i], right[i])) return 0;
    }
    return 1;
}

void FreeAddressHashTable(AddressHashTable *table)
{
    if (!table) return;
    for (size_t i = 0; i < table->count; ++i)
        FreeAddressKey(&table->entries[i].key);
    free(table->entries);
    free(table->buckets);
    free(table);
}

AddressHashTable *BuildAddressHashTable(const DataAddress *addresses, size_t count)
{
    if ((!addresses && count) || count > (SIZE_MAX - 1) / 2) return NULL;
    size_t buckets = count * 2 + 1;
    if (buckets > SIZE_MAX / sizeof(size_t) ||
        count > SIZE_MAX / sizeof(AddressHashEntry)) return NULL;
    AddressHashTable *table = calloc(1, sizeof(*table));
    if (!table) return NULL;
    table->bucketCount = buckets;
    table->buckets = malloc(buckets * sizeof(*table->buckets));
    if (!table->buckets) { FreeAddressHashTable(table); return NULL; }
    for (size_t i = 0; i < buckets; ++i) table->buckets[i] = SIZE_MAX;
    if (count) {
        table->entries = calloc(count, sizeof(*table->entries));
        if (!table->entries) { FreeAddressHashTable(table); return NULL; }
    }
    for (size_t i = 0; i < count; ++i) {
        AddressHashEntry *entry = &table->entries[i];
        if (!MakeAddressKey(&addresses[i], &entry->key)) {
            fprintf(stderr, "Address key failed at row %zu (invalid UTF-8 or allocation failure).\n", i + 1);
            FreeAddressHashTable(table);
            return NULL;
        }
        table->count++;
        entry->ID = addresses[i].ID;
        entry->hash = HashAddressKey(&entry->key);
        size_t bucket = (size_t)(entry->hash % buckets);
        entry->next = table->buckets[bucket];
        table->buckets[bucket] = i;
    }
    return table;
}

AddressMatch FindAddressID(const AddressHashTable *table, const DataAddress *search)
{
    AddressMatch result = {.status = ADDRESS_ERROR};
    if (!table || !search || !table->bucketCount) return result;
    AddressKey key;
    if (!MakeAddressKey(search, &key)) return result;
    uint64_t hash = HashAddressKey(&key);
    size_t bucket = (size_t)(hash % table->bucketCount);
    size_t matches = 0;
    for (size_t i = table->buckets[bucket]; i != SIZE_MAX; i = table->entries[i].next)
        if (table->entries[i].hash == hash && AddressKeysEqual(&key, &table->entries[i].key))
            matches++;
    if (matches) {
        if (matches > SIZE_MAX / sizeof(*result.IDs)) { FreeAddressKey(&key); return result; }
        result.IDs = malloc(matches * sizeof(*result.IDs));
        if (!result.IDs) { FreeAddressKey(&key); return result; }
        for (size_t i = table->buckets[bucket]; i != SIZE_MAX; i = table->entries[i].next)
            if (table->entries[i].hash == hash && AddressKeysEqual(&key, &table->entries[i].key))
                result.IDs[result.count++] = table->entries[i].ID;
    }
    result.status = matches > 1 ? ADDRESS_DUPLICATE : matches ? ADDRESS_FOUND : ADDRESS_NOT_FOUND;
    FreeAddressKey(&key);
    return result;
}



int FindAddressInDB(MYSQL *db, const DataAddress *search)
{
    if (db == NULL || search == NULL) {
        return EXIT_FAILURE;
    }

    const char *query =
        "SELECT DataAddress_ID "
        "FROM RepMyBlock.DataAddress "
        "WHERE DataAddress_HouseNumber <=> ? "
        "AND DataAddress_FracAddress <=> ? "
        "AND DataAddress_PreStreet <=> ? "
        "AND DataStreet_ID <=> ? "
        "AND DataAddress_PostStreet <=> ? "
        "AND DataCity_ID <=> ? "
        "AND DataCounty_ID <=> ? "
        "AND DataAddress_zipcode <=> ? "
        "AND DataAddress_zip4 <=> ?";

    MYSQL_STMT *stmt = mysql_stmt_init(db);
    if (stmt == NULL) {
        fprintf(stderr, "Statement initialization failed: %s\n",
                mysql_error(db));
        return EXIT_FAILURE;
    }

    int status = EXIT_FAILURE;
    MYSQL_BIND parameters[9] = {0};
    bool isNull[9] = {0};
    unsigned long lengths[9] = {0};

    /*
     * Parameters follow database columns 1 through 9,
     * matching the bit positions in nullFields.
     */
    for (size_t i = 0; i < 9; ++i) {
        isNull[i] = (search->nullFields & (1u << (i + 1))) != 0;
        parameters[i].is_null = &isNull[i];
    }

    const size_t textIndexes[] = {0, 1, 2, 4, 7, 8};

    const char *texts[] = {
        search->HouseNumber,
        search->FracAddress,
        search->PreStreet,
        search->PostStreet,
        search->zipcode,
        search->zip4
    };

    for (size_t i = 0; i < 6; ++i) {
        size_t parameter = textIndexes[i];

        lengths[parameter] = isNull[parameter]
            ? 0 : (unsigned long)strlen(texts[i]);

        parameters[parameter].buffer_type = MYSQL_TYPE_STRING;
        parameters[parameter].buffer = (void *)texts[i];
        parameters[parameter].buffer_length = lengths[parameter];
        parameters[parameter].length = &lengths[parameter];
    }

    /* Local copies remain valid until execution finishes. */
    uint32_t numbers[] = {
        search->DataStreet_ID,
        search->DataCity_ID,
        search->DataCounty_ID
    };

    const size_t numericIndexes[] = {3, 5, 6};

    for (size_t i = 0; i < 3; ++i) {
        size_t parameter = numericIndexes[i];

        parameters[parameter].buffer_type = MYSQL_TYPE_LONG;
        parameters[parameter].buffer = &numbers[i];
        parameters[parameter].buffer_length = sizeof(numbers[i]);
        parameters[parameter].is_unsigned = true;
    }

    if (mysql_stmt_prepare(stmt, query,
                           (unsigned long)strlen(query)) != 0 ||
        mysql_stmt_bind_param(stmt, parameters) != 0 ||
        mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Address query failed: %s\n",
                mysql_stmt_error(stmt));
        goto cleanup;
    }

    uint32_t addressID = 0;
    MYSQL_BIND output[1] = {0};

    output[0].buffer_type = MYSQL_TYPE_LONG;
    output[0].buffer = &addressID;
    output[0].buffer_length = sizeof(addressID);
    output[0].is_unsigned = true;

    if (mysql_stmt_bind_result(stmt, output) != 0 ||
        mysql_stmt_store_result(stmt) != 0) {
        fprintf(stderr, "Address result failed: %s\n",
                mysql_stmt_error(stmt));
        goto cleanup;
    }

    my_ulonglong matches = mysql_stmt_num_rows(stmt);

    if (matches == 0) {
        printf("Database: address not found.\n");
    } else if (matches > 1) {
        printf("Database: duplicate address (%llu matches).\n",
               (unsigned long long)matches);
    }

    int fetchStatus;

    while ((fetchStatus = mysql_stmt_fetch(stmt)) == 0) {
        printf("Database address ID: %" PRIu32 "\n", addressID);
    }

    if (fetchStatus != MYSQL_NO_DATA) {
        fprintf(stderr, "Address fetch failed: %s\n",
                mysql_stmt_error(stmt));
        goto cleanup;
    }

    status = EXIT_SUCCESS;

cleanup:
    mysql_stmt_close(stmt);
    return status;
}