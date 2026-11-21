#ifndef DATAPACCANDIDATE_H
#define DATAPACCANDIDATE_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t CandidateProfile_ID;
    uint32_t DataPAC_ID;
} DataPACCandidate;

typedef struct PACCandidateHashTable PACCandidateHashTable;
typedef RecordMatch PACCandidateMatch;
typedef RecordLookupResult PACCandidateLookupResult;
#define PACCANDIDATE_ERROR RECORD_ERROR
#define PACCANDIDATE_NOT_FOUND RECORD_NOT_FOUND
#define PACCANDIDATE_FOUND RECORD_FOUND
#define PACCANDIDATE_DUPLICATE RECORD_DUPLICATE

int LoadPACCandidateIntoMemory3(MYSQL *, DataPACCandidate **, size_t *);
int LoadPACCandidateIntoMemory4(MYSQL *, DataPACCandidate **, size_t *, size_t);
#define PACCandidate_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadPACCandidateIntoMemory(...) PACCandidate_LOAD_SELECT(__VA_ARGS__, LoadPACCandidateIntoMemory4, LoadPACCandidateIntoMemory3)(__VA_ARGS__)
void PrintPACCandidate(const DataPACCandidate *);
int ParsePACCandidateQuery(char *, void *);
int ParsePACCandidateRecord(char *, void *);
PACCandidateHashTable *BuildPACCandidateHashTable(const DataPACCandidate *, size_t);
PACCandidateMatch FindPACCandidateID(const PACCandidateHashTable *, const DataPACCandidate *);
void FreePACCandidateMatch(PACCandidateMatch *);
void FreePACCandidateHashTable(PACCandidateHashTable *);
int QueryPACCandidateMemory(const void *, const void *);
int QueryPACCandidateDatabase(MYSQL *, const void *);

#endif
