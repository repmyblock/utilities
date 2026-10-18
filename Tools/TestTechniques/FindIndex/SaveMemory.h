#ifndef SAVE_MEMORY_H
#define SAVE_MEMORY_H

#include <stddef.h>
#include <stdio.h>

#include "DataAddress.h"
#include "DataCounty.h"
#include "DataDistrict.h"
#include "DataDistrictCycle.h"
#include "DataDistrictTemporal.h"
#include "DataHouse.h"
#include "Voter.h"
#include "VoterComplementInfo.h"
#include "VoterIndex.h"

/*
 * Person and NameHashTable should be declared before including
 * this header, or placed in a shared header.
 */
typedef struct {
	int id;
	char name[255];
} SaveMemoryPerson;

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
						    const SaveMemoryPerson *lastNames, size_t lastNameCount);

int LoadMemory(const char *filename,
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
						    Person **lastNames, size_t *lastNameCount);
#endif