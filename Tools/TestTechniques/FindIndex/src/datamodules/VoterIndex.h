#ifndef VOTERINDEX_H
#define VOTERINDEX_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t DataLastName_ID;
    uint32_t DataFirstName_ID;
    uint32_t DataMiddleName_ID;
    char Suffix[41];
    char DOB[16];
    uint32_t DataState_ID;
    char UniqStateVoterID[81];
} VoterIndex;

typedef struct VoterIndexHashTable VoterIndexHashTable;
typedef RecordMatch VoterIndexMatch;
typedef RecordLookupResult VoterIndexLookupResult;
#define VOTERINDEX_ERROR RECORD_ERROR
#define VOTERINDEX_NOT_FOUND RECORD_NOT_FOUND
#define VOTERINDEX_FOUND RECORD_FOUND
#define VOTERINDEX_DUPLICATE RECORD_DUPLICATE

int LoadVoterIndexIntoMemory3(MYSQL *, VoterIndex **, size_t *);
int LoadVoterIndexIntoMemory4(MYSQL *, VoterIndex **, size_t *, size_t);
#define VoterIndex_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadVoterIndexIntoMemory(...) VoterIndex_LOAD_SELECT(__VA_ARGS__, LoadVoterIndexIntoMemory4, LoadVoterIndexIntoMemory3)(__VA_ARGS__)
void PrintVoterIndex(const VoterIndex *);
int ParseVoterIndexQuery(char *, void *);
int ParseVoterIndexRecord(char *, void *);
VoterIndexHashTable *BuildVoterIndexHashTable(const VoterIndex *, size_t);
VoterIndexMatch FindVoterIndexID(const VoterIndexHashTable *, const VoterIndex *);
void FreeVoterIndexMatch(VoterIndexMatch *);
void FreeVoterIndexHashTable(VoterIndexHashTable *);
int QueryVoterIndexMemory(const void *, const void *);
int QueryVoterIndexDatabase(MYSQL *, const void *);

#endif
