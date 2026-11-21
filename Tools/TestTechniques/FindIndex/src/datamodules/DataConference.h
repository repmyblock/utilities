#ifndef DATACONFERENCE_H
#define DATACONFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
    char Abbrev[161];
    char Web[1021];
    char LogoPath[1021];
    uint32_t Left_ID;
    uint32_t Right_ID;
    char CandidateElection_DBTable[1025];
    uint32_t DataInter_ID;
    char ValidFrom[40];
    char ValidTo[40];
} DataConference;

typedef struct ConferenceHashTable ConferenceHashTable;
typedef RecordMatch ConferenceMatch;
typedef RecordLookupResult ConferenceLookupResult;
#define CONFERENCE_ERROR RECORD_ERROR
#define CONFERENCE_NOT_FOUND RECORD_NOT_FOUND
#define CONFERENCE_FOUND RECORD_FOUND
#define CONFERENCE_DUPLICATE RECORD_DUPLICATE

int LoadConferenceIntoMemory3(MYSQL *, DataConference **, size_t *);
int LoadConferenceIntoMemory4(MYSQL *, DataConference **, size_t *, size_t);
#define Conference_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadConferenceIntoMemory(...) Conference_LOAD_SELECT(__VA_ARGS__, LoadConferenceIntoMemory4, LoadConferenceIntoMemory3)(__VA_ARGS__)
void PrintConference(const DataConference *);
int ParseConferenceQuery(char *, void *);
int ParseConferenceRecord(char *, void *);
ConferenceHashTable *BuildConferenceHashTable(const DataConference *, size_t);
ConferenceMatch FindConferenceID(const ConferenceHashTable *, const DataConference *);
void FreeConferenceMatch(ConferenceMatch *);
void FreeConferenceHashTable(ConferenceHashTable *);
int QueryConferenceMemory(const void *, const void *);
int QueryConferenceDatabase(MYSQL *, const void *);

#endif
