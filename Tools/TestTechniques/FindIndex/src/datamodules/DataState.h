#ifndef DATASTATE_H
#define DATASTATE_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
    char Abbrev[9];
    char Country[9];
} DataState;

typedef struct StateHashTable StateHashTable;
typedef RecordMatch StateMatch;
typedef RecordLookupResult StateLookupResult;
#define STATE_ERROR RECORD_ERROR
#define STATE_NOT_FOUND RECORD_NOT_FOUND
#define STATE_FOUND RECORD_FOUND
#define STATE_DUPLICATE RECORD_DUPLICATE

int LoadStateIntoMemory3(MYSQL *, DataState **, size_t *);
int LoadStateIntoMemory4(MYSQL *, DataState **, size_t *, size_t);
#define State_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadStateIntoMemory(...) State_LOAD_SELECT(__VA_ARGS__, LoadStateIntoMemory4, LoadStateIntoMemory3)(__VA_ARGS__)
void PrintState(const DataState *);
int ParseStateQuery(char *, void *);
int ParseStateRecord(char *, void *);
StateHashTable *BuildStateHashTable(const DataState *, size_t);
StateMatch FindStateID(const StateHashTable *, const DataState *);
void FreeStateMatch(StateMatch *);
void FreeStateHashTable(StateHashTable *);
int QueryStateMemory(const void *, const void *);
int QueryStateDatabase(MYSQL *, const void *);

#endif
