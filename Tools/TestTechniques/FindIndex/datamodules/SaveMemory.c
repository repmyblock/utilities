#include "SaveMemory.h"
#include "RecordLoader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_MAGIC "RMBMEM01"
#define SAVE_VERSION 1u

typedef struct {
  char magic[8];
  uint32_t version;
  uint32_t reserved;

  uint64_t addressCount;
  uint64_t countyCount;
  uint64_t districtCount;
  uint64_t districtCycleCount;
  uint64_t districtTemporalCount;
  uint64_t houseCount;
  uint64_t voterCount;
  uint64_t complementInfoCount;
  uint64_t voterIndexCount;
  uint64_t firstNameCount;
  uint64_t middleNameCount;
  uint64_t lastNameCount;
} SaveHeader;

#define WRITE_ARRAY(file, array, count)                          \
    do {                                                         \
        if ((count) > 0 &&                                       \
            fwrite((array), sizeof(*(array)), (count), (file))   \
                != (count)) {                                    \
            fprintf(stderr, "Error writing memory snapshot.\n"); \
            fclose(file);                                        \
            return EXIT_FAILURE;                                 \
        }                                                        \
    } while (0)

int SaveMemory(const char *filename,
							 	const DataAddress *addresses, size_t addressCount,
							  const DataCounty *counties, size_t countyCount,
							  const DataDistrict *districts, size_t districtCount,
							  const DataDistrictCycle *districtCycles, size_t districtCycleCount,
							  const DataDistrictTemporal *districtTemporals, size_t districtTemporalCount,
							  const DataHouse *houses, size_t houseCount,
							  const Voter *voters, size_t voterCount,
							  const VoterComplementInfo *complementInfo, size_t complementInfoCount,
							  const VoterIndex *voterIndexes, size_t voterIndexCount,
							  const SaveMemoryPerson *firstNames, size_t firstNameCount,
							  const SaveMemoryPerson *middleNames, size_t middleNameCount,
							  const SaveMemoryPerson *lastNames, size_t lastNameCount) {
  if (filename == NULL) {
     return EXIT_FAILURE;
  }

  FILE *file = fopen(filename, "wb");

  if (file == NULL) {
    perror(filename);
    return EXIT_FAILURE;
  }

  SaveHeader header;
  memset(&header, 0, sizeof(header));
  memcpy(header.magic, SAVE_MAGIC, sizeof(header.magic));
  header.version = SAVE_VERSION;

  header.addressCount = addressCount;
  header.countyCount = countyCount;
  header.districtCount = districtCount;
  header.districtCycleCount = districtCycleCount;
  header.districtTemporalCount = districtTemporalCount;
  header.houseCount = houseCount;
  header.voterCount = voterCount;
  header.complementInfoCount = complementInfoCount;
  header.voterIndexCount = voterIndexCount;
  header.firstNameCount = firstNameCount;
  header.middleNameCount = middleNameCount;
  header.lastNameCount = lastNameCount;

  if (fwrite(&header, sizeof(header), 1, file) != 1) {
    fprintf(stderr, "Could not write snapshot header.\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  WRITE_ARRAY(file, addresses, addressCount);
  WRITE_ARRAY(file, counties, countyCount);
  WRITE_ARRAY(file, districts, districtCount);
  WRITE_ARRAY(file, districtCycles, districtCycleCount);
  WRITE_ARRAY(file, districtTemporals, districtTemporalCount);
  WRITE_ARRAY(file, houses, houseCount);
  WRITE_ARRAY(file, voters, voterCount);
  WRITE_ARRAY(file, complementInfo, complementInfoCount);
  WRITE_ARRAY(file, voterIndexes, voterIndexCount);
  WRITE_ARRAY(file, firstNames, firstNameCount);
  WRITE_ARRAY(file, middleNames, middleNameCount);
  WRITE_ARRAY(file, lastNames, lastNameCount);

  fclose(file);

  printf("Memory saved to %s\n", filename);
  return EXIT_SUCCESS;
}

#include "SaveMemory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_MAGIC   "RMBMEM01"
#define SAVE_VERSION 1u

typedef struct {
    char magic[8];
    uint32_t version;
    uint32_t reserved;

    uint64_t addressCount;
    uint64_t countyCount;
    uint64_t districtCount;
    uint64_t districtCycleCount;
    uint64_t districtTemporalCount;
    uint64_t houseCount;
    uint64_t voterCount;
    uint64_t complementInfoCount;
    uint64_t voterIndexCount;
    uint64_t firstNameCount;
    uint64_t middleNameCount;
    uint64_t lastNameCount;
} SaveHeader;

#define LOAD_ARRAY(file, array, count)                                \
    do {                                                              \
        if ((count) > 0) {                                            \
            (array) = malloc((count) * sizeof(*(array)));             \
            if ((array) == NULL) {                                    \
                fprintf(stderr, "Memory allocation failed.\n");       \
                goto failure;                                         \
            }                                                         \
                                                                      \
            if (fread((array), sizeof(*(array)), (count), (file))     \
                    != (count)) {                                     \
                fprintf(stderr, "Invalid or incomplete snapshot.\n"); \
                goto failure;                                         \
            }                                                         \
        } else {                                                      \
            (array) = NULL;                                           \
        }                                                             \
    } while (0)

#define FREE_LOADED()               \
    do {                            \
        free(*addresses);           \
        free(*counties);            \
        free(*districts);           \
        free(*districtCycles);      \
        free(*districtTemporals);   \
        free(*houses);              \
        free(*voters);              \
        free(*complementInfo);      \
        free(*voterIndexes);        \
        free(*firstNames);          \
        free(*middleNames);         \
        free(*lastNames);           \
    } while (0)

int LoadMemory( const char *filename,
						    DataAddress **addresses, size_t *addressCount,
						    DataCounty **counties, size_t *countyCount,
						    DataDistrict **districts, size_t *districtCount,
						    DataDistrictCycle **districtCycles, size_t *districtCycleCount,
						    DataDistrictTemporal **districtTemporals, size_t *districtTemporalCount,
						    DataHouse **houses, size_t *houseCount,
						    Voter **voters, size_t *voterCount,
						    VoterComplementInfo **complementInfo, size_t *complementInfoCount,
						    VoterIndex **voterIndexes, size_t *voterIndexCount,
						    Person **firstNames, size_t *firstNameCount,
						    Person **middleNames, size_t *middleNameCount,
						    Person **lastNames, size_t *lastNameCount) {
	if (filename == NULL || addresses == NULL || addressCount == NULL || counties == NULL || countyCount == NULL ||
      districts == NULL || districtCount == NULL || districtCycles == NULL || districtCycleCount == NULL ||
      districtTemporals == NULL || districtTemporalCount == NULL || houses == NULL || houseCount == NULL ||
      voters == NULL || voterCount == NULL || complementInfo == NULL || complementInfoCount == NULL ||
      voterIndexes == NULL || voterIndexCount == NULL || firstNames == NULL || firstNameCount == NULL ||
      middleNames == NULL || middleNameCount == NULL ||  lastNames == NULL || lastNameCount == NULL) {
		fprintf(stderr, "Invalid LoadMemory arguments.\n");
  	return EXIT_FAILURE;
  }

  *addresses = NULL;
  *counties = NULL;
  *districts = NULL;
  *districtCycles = NULL;
  *districtTemporals = NULL;
  *houses = NULL;
  *voters = NULL;
  *complementInfo = NULL;
  *voterIndexes = NULL;
  *firstNames = NULL;
  *middleNames = NULL;
  *lastNames = NULL;

  *addressCount = 0;
  *countyCount = 0;
  *districtCount = 0;
  *districtCycleCount = 0;
  *districtTemporalCount = 0;
  *houseCount = 0;
  *voterCount = 0;
  *complementInfoCount = 0;
  *voterIndexCount = 0;
  *firstNameCount = 0;
  *middleNameCount = 0;
  *lastNameCount = 0;

  FILE *file = fopen(filename, "rb");

  if (file == NULL) {
    perror(filename);
    return EXIT_FAILURE;
  }

  SaveHeader header;

  if (fread(&header, sizeof(header), 1, file) != 1) {
    fprintf(stderr, "Could not read snapshot header.\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  if (memcmp(header.magic, SAVE_MAGIC, sizeof(header.magic)) != 0 ||
    header.version != SAVE_VERSION) {
    fprintf(stderr, "Invalid or unsupported snapshot file.\n");
    fclose(file);
    return EXIT_FAILURE;
  }

#define CHECK_SIZE(value) ((value) > SIZE_MAX)
  if (CHECK_SIZE(header.addressCount) || CHECK_SIZE(header.countyCount) ||
      CHECK_SIZE(header.districtCount) || CHECK_SIZE(header.districtCycleCount) ||
      CHECK_SIZE(header.districtTemporalCount) || CHECK_SIZE(header.houseCount) ||
      CHECK_SIZE(header.voterCount) || CHECK_SIZE(header.complementInfoCount) ||
      CHECK_SIZE(header.voterIndexCount) || CHECK_SIZE(header.firstNameCount) ||
      CHECK_SIZE(header.middleNameCount) || CHECK_SIZE(header.lastNameCount)) {
    fprintf(stderr, "Snapshot contains invalid counts.\n");
    fclose(file);
    return EXIT_FAILURE;
  }
#undef CHECK_SIZE

  *addressCount = (size_t)header.addressCount;
  *countyCount = (size_t)header.countyCount;
  *districtCount = (size_t)header.districtCount;
  *districtCycleCount = (size_t)header.districtCycleCount;
  *districtTemporalCount = (size_t)header.districtTemporalCount;
  *houseCount = (size_t)header.houseCount;
  *voterCount = (size_t)header.voterCount;
  *complementInfoCount = (size_t)header.complementInfoCount;
  *voterIndexCount = (size_t)header.voterIndexCount;
  *firstNameCount = (size_t)header.firstNameCount;
  *middleNameCount = (size_t)header.middleNameCount;
  *lastNameCount = (size_t)header.lastNameCount;

  LOAD_ARRAY(file, *addresses, *addressCount);
  LOAD_ARRAY(file, *counties, *countyCount);
  LOAD_ARRAY(file, *districts, *districtCount);
  LOAD_ARRAY(file, *districtCycles, *districtCycleCount);
  LOAD_ARRAY(file, *districtTemporals, *districtTemporalCount);
  LOAD_ARRAY(file, *houses, *houseCount);
  LOAD_ARRAY(file, *voters, *voterCount);
  LOAD_ARRAY(file, *complementInfo, *complementInfoCount);
  LOAD_ARRAY(file, *voterIndexes, *voterIndexCount);
  LOAD_ARRAY(file, *firstNames, *firstNameCount);
  LOAD_ARRAY(file, *middleNames, *middleNameCount);
  LOAD_ARRAY(file, *lastNames, *lastNameCount);

  fclose(file);

  printf("Memory loaded from %s\n", filename);
  return EXIT_SUCCESS;

failure:
  fclose(file);
  FREE_LOADED();

  *addresses = NULL;
  *counties = NULL;
  *districts = NULL;
  *districtCycles = NULL;
  *districtTemporals = NULL;
  *houses = NULL;
  *voters = NULL;
  *complementInfo = NULL;
  *voterIndexes = NULL;
  *firstNames = NULL;
  *middleNames = NULL;
  *lastNames = NULL;

  *addressCount = 0;
  *countyCount = 0;
  *districtCount = 0;
  *districtCycleCount = 0;
  *districtTemporalCount = 0;
  *houseCount = 0;
  *voterCount = 0;
  *complementInfoCount = 0;
  *voterIndexCount = 0;
  *firstNameCount = 0;
  *middleNameCount = 0;
  *lastNameCount = 0;

  return EXIT_FAILURE;
}

#undef LOAD_ARRAY
#undef FREE_LOADED

#undef WRITE_ARRAY