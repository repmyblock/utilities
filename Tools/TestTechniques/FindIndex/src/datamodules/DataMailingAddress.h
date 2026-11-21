#ifndef DATAMAILINGADDRESS_H
#define DATAMAILINGADDRESS_H

#include <stddef.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include "../RecordHash.h"

typedef struct {
    uint32_t ID;
    /* RecordLoader writes a uint32_t null mask at this offset. */
    uint32_t nullFields;
    char Line1[1025];
    char Line2[1025];
    char Line3[1025];
    char Line4[1025];
} DataMailingAddress;

typedef struct MailingAddressHashTable MailingAddressHashTable;
typedef RecordMatch MailingAddressMatch;
typedef RecordLookupResult MailingAddressLookupResult;
#define MAILINGADDRESS_ERROR RECORD_ERROR
#define MAILINGADDRESS_NOT_FOUND RECORD_NOT_FOUND
#define MAILINGADDRESS_FOUND RECORD_FOUND
#define MAILINGADDRESS_DUPLICATE RECORD_DUPLICATE

int LoadMailingAddressIntoMemory3(MYSQL *, DataMailingAddress **, size_t *);
int LoadMailingAddressIntoMemory4(MYSQL *, DataMailingAddress **, size_t *, size_t);
#define MailingAddress_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME
#define LoadMailingAddressIntoMemory(...) MailingAddress_LOAD_SELECT(__VA_ARGS__, LoadMailingAddressIntoMemory4, LoadMailingAddressIntoMemory3)(__VA_ARGS__)
void PrintMailingAddress(const DataMailingAddress *);
int ParseMailingAddressQuery(char *, void *);
int ParseMailingAddressRecord(char *, void *);
MailingAddressHashTable *BuildMailingAddressHashTable(const DataMailingAddress *, size_t);
MailingAddressMatch FindMailingAddressID(const MailingAddressHashTable *, const DataMailingAddress *);
void FreeMailingAddressMatch(MailingAddressMatch *);
void FreeMailingAddressHashTable(MailingAddressHashTable *);
int QueryMailingAddressMemory(const void *, const void *);
int QueryMailingAddressDatabase(MYSQL *, const void *);

#endif
