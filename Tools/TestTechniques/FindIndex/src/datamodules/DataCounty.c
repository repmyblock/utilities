#include "DataCounty.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CountyHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataCounty, DataState_ID), sizeof(((DataCounty *)0)->DataState_ID), 0},
    {offsetof(DataCounty, Name), sizeof(((DataCounty *)0)->Name), 1},
    {offsetof(DataCounty, BOEID), sizeof(((DataCounty *)0)->BOEID), 0},
    {offsetof(DataCounty, GEOID), sizeof(((DataCounty *)0)->GEOID), 0},
};
static const RecordField keyFields[] = {
    {offsetof(DataCounty, DataState_ID), sizeof(((DataCounty *)0)->DataState_ID), 0},
    {offsetof(DataCounty, Name), sizeof(((DataCounty *)0)->Name), 1},
    {offsetof(DataCounty, BOEID), sizeof(((DataCounty *)0)->BOEID), 0},
    {offsetof(DataCounty, GEOID), sizeof(((DataCounty *)0)->GEOID), 0},
};

static int LoadCountyRows(MYSQL *db, DataCounty **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataCounty_ID`, `DataState_ID`, `DataCounty_Name`, `DataCounty_BOEID`, `DataCounty_GEOID` FROM `DataCounty`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataCounty), offsetof(DataCounty, ID), offsetof(DataCounty, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadCountyIntoMemory3(MYSQL *db, DataCounty **records, size_t *count) { return LoadCountyRows(db, records, count, 0); }
int LoadCountyIntoMemory4(MYSQL *db, DataCounty **records, size_t *count, size_t limit) { return LoadCountyRows(db, records, count, limit); }

void PrintCounty(const DataCounty *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" DataState_ID=%" PRIu32, record->DataState_ID);
    printf(" Name=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->Name);
    printf(" BOEID=%" PRIu32, record->BOEID);
    printf(" GEOID=%" PRIu32, record->GEOID);
    putchar('\n'); }

int ParseCountyQuery(char *line, void *opaqueRecord) {
    DataCounty *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for County\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataState_ID: %s\n", token0); return EXIT_FAILURE; } record->DataState_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for County\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->Name) { fprintf(stderr, "Name exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Name, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for County\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        errno = 0; char *end2 = NULL; uintmax_t value2 = strtoumax(token2, &end2, 10); if (errno || end2 == token2 || *end2 || value2 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for BOEID: %s\n", token2); return EXIT_FAILURE; } record->BOEID = (uint32_t)value2;
    }
    char *token3 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for County\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        errno = 0; char *end3 = NULL; uintmax_t value3 = strtoumax(token3, &end3, 10); if (errno || end3 == token3 || *end3 || value3 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for GEOID: %s\n", token3); return EXIT_FAILURE; } record->GEOID = (uint32_t)value3;
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for County\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseCountyRecord(char *line, void *record) { return ParseCountyQuery(line, record); }

CountyHashTable *BuildCountyHashTable(const DataCounty *records, size_t count) {
    CountyHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataCounty, ID), offsetof(DataCounty, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
CountyMatch FindCountyID(const CountyHashTable *table, const DataCounty *record) { CountyMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeCountyMatch(CountyMatch *match) { FreeRecordMatch(match); }
void FreeCountyHashTable(CountyHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryCountyMemory(const void *opaque, const void *record) {
    const CountyHashTable *table = opaque; const DataCounty *query = record; if (!table || !query) return EXIT_FAILURE; CountyMatch m = FindCountyID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeCountyMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryCountyDatabase(MYSQL *db, const void *record) {
    const DataCounty *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataCounty_ID` FROM `DataCounty` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataState_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataState_ID` = %" PRIu32,q->DataState_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataCounty_Name` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataCounty_Name` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Name); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Name,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataCounty_BOEID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCounty_BOEID` = %" PRIu32,q->BOEID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataCounty_GEOID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCounty_GEOID` = %" PRIu32,q->GEOID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
