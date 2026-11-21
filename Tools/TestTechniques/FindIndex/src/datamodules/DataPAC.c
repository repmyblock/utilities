#include "DataPAC.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PACHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataPAC, Name), sizeof(((DataPAC *)0)->Name), 1},
    {offsetof(DataPAC, Web), sizeof(((DataPAC *)0)->Web), 1},
    {offsetof(DataPAC, LogoPath), sizeof(((DataPAC *)0)->LogoPath), 1},
    {offsetof(DataPAC, EuropeanModel), sizeof(((DataPAC *)0)->EuropeanModel), 1},
    {offsetof(DataPAC, AmericanModel), sizeof(((DataPAC *)0)->AmericanModel), 1},
    {offsetof(DataPAC, Type), sizeof(((DataPAC *)0)->Type), 1},
    {offsetof(DataPAC, Official), sizeof(((DataPAC *)0)->Official), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataPAC, Name), sizeof(((DataPAC *)0)->Name), 1},
    {offsetof(DataPAC, Web), sizeof(((DataPAC *)0)->Web), 1},
    {offsetof(DataPAC, LogoPath), sizeof(((DataPAC *)0)->LogoPath), 1},
    {offsetof(DataPAC, EuropeanModel), sizeof(((DataPAC *)0)->EuropeanModel), 1},
    {offsetof(DataPAC, AmericanModel), sizeof(((DataPAC *)0)->AmericanModel), 1},
    {offsetof(DataPAC, Type), sizeof(((DataPAC *)0)->Type), 1},
    {offsetof(DataPAC, Official), sizeof(((DataPAC *)0)->Official), 1},
};

static int LoadPACRows(MYSQL *db, DataPAC **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataPAC_ID`, `DataPAC_Name`, `DataPAC_Web`, `DataPAC_LogoPath`, `DataPAC_EuropeanModel`, `DataPAC_AmericanModel`, `DataPAC_Type`, `DataPAC_Official` FROM `DataPAC`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataPAC), offsetof(DataPAC, ID), offsetof(DataPAC, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadPACIntoMemory3(MYSQL *db, DataPAC **records, size_t *count) { return LoadPACRows(db, records, count, 0); }
int LoadPACIntoMemory4(MYSQL *db, DataPAC **records, size_t *count, size_t limit) { return LoadPACRows(db, records, count, limit); }

void PrintPAC(const DataPAC *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" Name=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->Name);
    printf(" Web=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->Web);
    printf(" LogoPath=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->LogoPath);
    printf(" EuropeanModel=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->EuropeanModel);
    printf(" AmericanModel=%s", (record->nullFields & (UINT32_C(1) << 5)) ? "<NULL>" : record->AmericanModel);
    printf(" Type=%s", (record->nullFields & (UINT32_C(1) << 6)) ? "<NULL>" : record->Type);
    printf(" Official=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->Official);
    putchar('\n'); }

int ParsePACQuery(char *line, void *opaqueRecord) {
    DataPAC *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->Name) { fprintf(stderr, "Name exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Name, token0, len0 + 1);
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->Web) { fprintf(stderr, "Web exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Web, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->LogoPath) { fprintf(stderr, "LogoPath exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->LogoPath, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->EuropeanModel) { fprintf(stderr, "EuropeanModel exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->EuropeanModel, token3, len3 + 1);
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        size_t len4 = strlen(token4); if (len4 >= sizeof record->AmericanModel) { fprintf(stderr, "AmericanModel exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->AmericanModel, token4, len4 + 1);
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for PAC\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        size_t len5 = strlen(token5); if (len5 >= sizeof record->Type) { fprintf(stderr, "Type exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Type, token5, len5 + 1);
    }
    char *token6 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for PAC\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->Official) { fprintf(stderr, "Official exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Official, token6, len6 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for PAC\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParsePACRecord(char *line, void *record) { return ParsePACQuery(line, record); }

PACHashTable *BuildPACHashTable(const DataPAC *records, size_t count) {
    PACHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataPAC, ID), offsetof(DataPAC, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
PACMatch FindPACID(const PACHashTable *table, const DataPAC *record) { PACMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreePACMatch(PACMatch *match) { FreeRecordMatch(match); }
void FreePACHashTable(PACHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryPACMemory(const void *opaque, const void *record) {
    const PACHashTable *table = opaque; const DataPAC *query = record; if (!table || !query) return EXIT_FAILURE; PACMatch m = FindPACID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreePACMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryPACDatabase(MYSQL *db, const void *record) {
    const DataPAC *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataPAC_ID` FROM `DataPAC` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataPAC_Name` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_Name` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Name); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Name,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataPAC_Web` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_Web` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Web); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Web,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataPAC_LogoPath` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_LogoPath` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->LogoPath); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->LogoPath,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataPAC_EuropeanModel` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_EuropeanModel` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->EuropeanModel); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->EuropeanModel,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataPAC_AmericanModel` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_AmericanModel` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->AmericanModel); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->AmericanModel,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataPAC_Type` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_Type` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Type); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Type,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`DataPAC_Official` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPAC_Official` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Official); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Official,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
