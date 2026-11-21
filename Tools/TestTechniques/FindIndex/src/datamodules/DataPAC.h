#ifndef DATAPAC_H
#define DATAPAC_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Name[1021];
    char Web[1021];
    char LogoPath[1021];
    char EuropeanModel[21];
    char AmericanModel[21];
    char Type[25];
    char Official[21];
} DataPAC;

typedef struct PACHashTable PACHashTable;
typedef RecordMatch PACMatch;
typedef RecordLookupResult PACLookupResult;
#define PAC_ERROR RECORD_ERROR
#define PAC_NOT_FOUND RECORD_NOT_FOUND
#define PAC_FOUND RECORD_FOUND
#define PAC_DUPLICATE RECORD_DUPLICATE

int LoadPACIntoMemory3(MYSQL *, DataPAC **, size_t *);
int LoadPACIntoMemory4(MYSQL *, DataPAC **, size_t *, size_t);
#define PAC_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadPACIntoMemory(...) PAC_LOAD_SELECT(__VA_ARGS__, LoadPACIntoMemory4, LoadPACIntoMemory3)(__VA_ARGS__)
void PrintPAC(const DataPAC *);
int ParsePACQuery(char *, void *);
int ParsePACRecord(char *, void *);
PACHashTable *BuildPACHashTable(const DataPAC *, size_t);
PACMatch FindPACID(const PACHashTable *, const DataPAC *);
void FreePACMatch(PACMatch *);
void FreePACHashTable(PACHashTable *);
int QueryPACMemory(const void *, const void *);
int QueryPACDatabase(MYSQL *, const void *);

#endif
