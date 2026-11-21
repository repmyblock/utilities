#ifndef DATACLUB_H
#define DATACLUB_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    uint32_t State_ID;
    uint32_t DataCounty_ID;
    uint32_t DataParty_ID;
    char Name[1025];
    char Web[1025];
    char Email[1025];
    char enum_field[21];
    char LastVerified[40];
} DataClub;

typedef struct ClubHashTable ClubHashTable;
typedef RecordMatch ClubMatch;
typedef RecordLookupResult ClubLookupResult;
#define CLUB_ERROR RECORD_ERROR
#define CLUB_NOT_FOUND RECORD_NOT_FOUND
#define CLUB_FOUND RECORD_FOUND
#define CLUB_DUPLICATE RECORD_DUPLICATE

int LoadClubIntoMemory3(MYSQL *, DataClub **, size_t *);
int LoadClubIntoMemory4(MYSQL *, DataClub **, size_t *, size_t);
#define Club_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadClubIntoMemory(...) Club_LOAD_SELECT(__VA_ARGS__, LoadClubIntoMemory4, LoadClubIntoMemory3)(__VA_ARGS__)
void PrintClub(const DataClub *);
int ParseClubQuery(char *, void *);
int ParseClubRecord(char *, void *);
ClubHashTable *BuildClubHashTable(const DataClub *, size_t);
ClubMatch FindClubID(const ClubHashTable *, const DataClub *);
void FreeClubMatch(ClubMatch *);
void FreeClubHashTable(ClubHashTable *);
int QueryClubMemory(const void *, const void *);
int QueryClubDatabase(MYSQL *, const void *);

#endif
