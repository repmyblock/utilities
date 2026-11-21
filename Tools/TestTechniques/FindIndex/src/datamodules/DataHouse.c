#include "DataHouse.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HouseHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataHouse, DataAddress_ID), sizeof(((DataHouse *)0)->DataAddress_ID), 0},
    {offsetof(DataHouse, Type), sizeof(((DataHouse *)0)->Type), 1},
    {offsetof(DataHouse, Apt), sizeof(((DataHouse *)0)->Apt), 1},
    {offsetof(DataHouse, DataStreetNonStdFormat_ID), sizeof(((DataHouse *)0)->DataStreetNonStdFormat_ID), 0},
    {offsetof(DataHouse, BIN), sizeof(((DataHouse *)0)->BIN), 0},
};
static const RecordField keyFields[] = {
    {offsetof(DataHouse, DataAddress_ID), sizeof(((DataHouse *)0)->DataAddress_ID), 0},
    {offsetof(DataHouse, Type), sizeof(((DataHouse *)0)->Type), 1},
    {offsetof(DataHouse, Apt), sizeof(((DataHouse *)0)->Apt), 1},
    {offsetof(DataHouse, DataStreetNonStdFormat_ID), sizeof(((DataHouse *)0)->DataStreetNonStdFormat_ID), 0},
    {offsetof(DataHouse, BIN), sizeof(((DataHouse *)0)->BIN), 0},
};

static int LoadHouseRows(MYSQL *db, DataHouse **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataHouse_ID`, `DataAddress_ID`, `DataHouse_Type`, `DataHouse_Apt`, `DataStreetNonStdFormat_ID`, `DataHouse_BIN` FROM `DataHouse`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataHouse), offsetof(DataHouse, ID), offsetof(DataHouse, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadHouseIntoMemory3(MYSQL *db, DataHouse **records, size_t *count) { return LoadHouseRows(db, records, count, 0); }
int LoadHouseIntoMemory4(MYSQL *db, DataHouse **records, size_t *count, size_t limit) { return LoadHouseRows(db, records, count, limit); }

void PrintHouse(const DataHouse *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" DataAddress_ID=%" PRIu32, record->DataAddress_ID);
    printf(" Type=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->Type);
    printf(" Apt=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->Apt);
    printf(" DataStreetNonStdFormat_ID=%" PRIu32, record->DataStreetNonStdFormat_ID);
    printf(" BIN=%" PRIu32, record->BIN);
    putchar('\n'); }

int ParseHouseQuery(char *line, void *opaqueRecord) {
    DataHouse *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for House\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataAddress_ID: %s\n", token0); return EXIT_FAILURE; } record->DataAddress_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for House\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->Type) { fprintf(stderr, "Type exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Type, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for House\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->Apt) { fprintf(stderr, "Apt exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Apt, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for House\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        errno = 0; char *end3 = NULL; uintmax_t value3 = strtoumax(token3, &end3, 10); if (errno || end3 == token3 || *end3 || value3 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataStreetNonStdFormat_ID: %s\n", token3); return EXIT_FAILURE; } record->DataStreetNonStdFormat_ID = (uint32_t)value3;
    }
    char *token4 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for House\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        errno = 0; char *end4 = NULL; uintmax_t value4 = strtoumax(token4, &end4, 10); if (errno || end4 == token4 || *end4 || value4 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for BIN: %s\n", token4); return EXIT_FAILURE; } record->BIN = (uint32_t)value4;
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for House\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseHouseRecord(char *line, void *record) { return ParseHouseQuery(line, record); }

HouseHashTable *BuildHouseHashTable(const DataHouse *records, size_t count) {
    HouseHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataHouse, ID), offsetof(DataHouse, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
HouseMatch FindHouseID(const HouseHashTable *table, const DataHouse *record) { HouseMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeHouseMatch(HouseMatch *match) { FreeRecordMatch(match); }
void FreeHouseHashTable(HouseHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryHouseMemory(const void *opaque, const void *record) {
    const HouseHashTable *table = opaque; const DataHouse *query = record; if (!table || !query) return EXIT_FAILURE; HouseMatch m = FindHouseID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeHouseMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryHouseDatabase(MYSQL *db, const void *record) {
    const DataHouse *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataHouse_ID` FROM `DataHouse` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataAddress_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataAddress_ID` = %" PRIu32,q->DataAddress_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataHouse_Type` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataHouse_Type` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Type); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Type,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataHouse_Apt` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataHouse_Apt` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Apt); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Apt,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataStreetNonStdFormat_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataStreetNonStdFormat_ID` = %" PRIu32,q->DataStreetNonStdFormat_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataHouse_BIN` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataHouse_BIN` = %" PRIu32,q->BIN); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
