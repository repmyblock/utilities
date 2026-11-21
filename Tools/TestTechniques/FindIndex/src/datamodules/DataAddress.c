#include "DataAddress.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct AddressHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataAddress, HouseNumber), sizeof(((DataAddress *)0)->HouseNumber), 1},
    {offsetof(DataAddress, FracAddress), sizeof(((DataAddress *)0)->FracAddress), 1},
    {offsetof(DataAddress, PreStreet), sizeof(((DataAddress *)0)->PreStreet), 1},
    {offsetof(DataAddress, DataStreet_ID), sizeof(((DataAddress *)0)->DataStreet_ID), 0},
    {offsetof(DataAddress, PostStreet), sizeof(((DataAddress *)0)->PostStreet), 1},
    {offsetof(DataAddress, DataCity_ID), sizeof(((DataAddress *)0)->DataCity_ID), 0},
    {offsetof(DataAddress, DataCounty_ID), sizeof(((DataAddress *)0)->DataCounty_ID), 0},
    {offsetof(DataAddress, zipcode), sizeof(((DataAddress *)0)->zipcode), 1},
    {offsetof(DataAddress, zip4), sizeof(((DataAddress *)0)->zip4), 1},
    {offsetof(DataAddress, Cordinate_ID), sizeof(((DataAddress *)0)->Cordinate_ID), 0},
    {offsetof(DataAddress, PG_OSM_osmid), sizeof(((DataAddress *)0)->PG_OSM_osmid), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataAddress, HouseNumber), sizeof(((DataAddress *)0)->HouseNumber), 1},
    {offsetof(DataAddress, FracAddress), sizeof(((DataAddress *)0)->FracAddress), 1},
    {offsetof(DataAddress, PreStreet), sizeof(((DataAddress *)0)->PreStreet), 1},
    {offsetof(DataAddress, DataStreet_ID), sizeof(((DataAddress *)0)->DataStreet_ID), 0},
    {offsetof(DataAddress, PostStreet), sizeof(((DataAddress *)0)->PostStreet), 1},
    {offsetof(DataAddress, DataCity_ID), sizeof(((DataAddress *)0)->DataCity_ID), 0},
    {offsetof(DataAddress, DataCounty_ID), sizeof(((DataAddress *)0)->DataCounty_ID), 0},
    {offsetof(DataAddress, zipcode), sizeof(((DataAddress *)0)->zipcode), 1},
    {offsetof(DataAddress, zip4), sizeof(((DataAddress *)0)->zip4), 1},
};

static int LoadAddressesRows(MYSQL *db, DataAddress **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataAddress_ID`, `DataAddress_HouseNumber`, `DataAddress_FracAddress`, `DataAddress_PreStreet`, `DataStreet_ID`, `DataAddress_PostStreet`, `DataCity_ID`, `DataCounty_ID`, `DataAddress_zipcode`, `DataAddress_zip4`, `Cordinate_ID`, `PG_OSM_osmid` FROM `DataAddress`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataAddress), offsetof(DataAddress, ID), offsetof(DataAddress, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadAddressesIntoMemory3(MYSQL *db, DataAddress **records, size_t *count) { return LoadAddressesRows(db, records, count, 0); }
int LoadAddressesIntoMemory4(MYSQL *db, DataAddress **records, size_t *count, size_t limit) { return LoadAddressesRows(db, records, count, limit); }

void PrintAddress(const DataAddress *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" HouseNumber=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->HouseNumber);
    printf(" FracAddress=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->FracAddress);
    printf(" PreStreet=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->PreStreet);
    printf(" DataStreet_ID=%" PRIu32, record->DataStreet_ID);
    printf(" PostStreet=%s", (record->nullFields & (UINT32_C(1) << 5)) ? "<NULL>" : record->PostStreet);
    printf(" DataCity_ID=%" PRIu32, record->DataCity_ID);
    printf(" DataCounty_ID=%" PRIu32, record->DataCounty_ID);
    printf(" zipcode=%s", (record->nullFields & (UINT32_C(1) << 8)) ? "<NULL>" : record->zipcode);
    printf(" zip4=%s", (record->nullFields & (UINT32_C(1) << 9)) ? "<NULL>" : record->zip4);
    printf(" Cordinate_ID=%" PRIu32, record->Cordinate_ID);
    printf(" PG_OSM_osmid=%s", (record->nullFields & (UINT32_C(1) << 11)) ? "<NULL>" : record->PG_OSM_osmid);
    putchar('\n'); }

int ParseAddressQuery(char *line, void *opaqueRecord) {
    DataAddress *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->HouseNumber) { fprintf(stderr, "HouseNumber exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->HouseNumber, token0, len0 + 1);
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->FracAddress) { fprintf(stderr, "FracAddress exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->FracAddress, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->PreStreet) { fprintf(stderr, "PreStreet exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->PreStreet, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        errno = 0; char *end3 = NULL; uintmax_t value3 = strtoumax(token3, &end3, 10); if (errno || end3 == token3 || *end3 || value3 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataStreet_ID: %s\n", token3); return EXIT_FAILURE; } record->DataStreet_ID = (uint32_t)value3;
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        size_t len4 = strlen(token4); if (len4 >= sizeof record->PostStreet) { fprintf(stderr, "PostStreet exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->PostStreet, token4, len4 + 1);
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        errno = 0; char *end5 = NULL; uintmax_t value5 = strtoumax(token5, &end5, 10); if (errno || end5 == token5 || *end5 || value5 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataCity_ID: %s\n", token5); return EXIT_FAILURE; } record->DataCity_ID = (uint32_t)value5;
    }
    char *token6 = p; char *comma6 = strchr(p, ','); if (!comma6) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma6 = '\0'; p = comma6 + 1;
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        errno = 0; char *end6 = NULL; uintmax_t value6 = strtoumax(token6, &end6, 10); if (errno || end6 == token6 || *end6 || value6 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataCounty_ID: %s\n", token6); return EXIT_FAILURE; } record->DataCounty_ID = (uint32_t)value6;
    }
    char *token7 = p; char *comma7 = strchr(p, ','); if (!comma7) { fprintf(stderr, "Too few comma-separated fields for Address\n"); return EXIT_FAILURE; } *comma7 = '\0'; p = comma7 + 1;
    if (*token7 == '\0') { nulls |= UINT32_C(1) << 8; } else {
        size_t len7 = strlen(token7); if (len7 >= sizeof record->zipcode) { fprintf(stderr, "zipcode exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->zipcode, token7, len7 + 1);
    }
    char *token8 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Address\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token8 == '\0') { nulls |= UINT32_C(1) << 9; } else {
        size_t len8 = strlen(token8); if (len8 >= sizeof record->zip4) { fprintf(stderr, "zip4 exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->zip4, token8, len8 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Address\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseAddressRecord(char *line, void *record) { return ParseAddressQuery(line, record); }

AddressHashTable *BuildAddressHashTable(const DataAddress *records, size_t count) {
    AddressHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataAddress, ID), offsetof(DataAddress, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
AddressMatch FindAddressID(const AddressHashTable *table, const DataAddress *record) { AddressMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeAddressMatch(AddressMatch *match) { FreeRecordMatch(match); }
void FreeAddressHashTable(AddressHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryAddressMemory(const void *opaque, const void *record) {
    const AddressHashTable *table = opaque; const DataAddress *query = record; if (!table || !query) return EXIT_FAILURE; AddressMatch m = FindAddressID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeAddressMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryAddressDatabase(MYSQL *db, const void *record) {
    const DataAddress *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataAddress_ID` FROM `DataAddress` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataAddress_HouseNumber` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_HouseNumber` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->HouseNumber); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->HouseNumber,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataAddress_FracAddress` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_FracAddress` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->FracAddress); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->FracAddress,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataAddress_PreStreet` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_PreStreet` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->PreStreet); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->PreStreet,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataStreet_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataStreet_ID` = %" PRIu32,q->DataStreet_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataAddress_PostStreet` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_PostStreet` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->PostStreet); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->PostStreet,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataCity_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCity_ID` = %" PRIu32,q->DataCity_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`DataCounty_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCounty_ID` = %" PRIu32,q->DataCounty_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (7 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 8)) { const char *part = "`DataAddress_zipcode` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_zipcode` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->zipcode); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->zipcode,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (8 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 9)) { const char *part = "`DataAddress_zip4` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataAddress_zip4` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->zip4); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->zip4,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
