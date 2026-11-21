#include "DataInter.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct InterHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataInter, Name), sizeof(((DataInter *)0)->Name), 1},
    {offsetof(DataInter, Abbrev), sizeof(((DataInter *)0)->Abbrev), 1},
    {offsetof(DataInter, Web), sizeof(((DataInter *)0)->Web), 1},
    {offsetof(DataInter, LogoPath), sizeof(((DataInter *)0)->LogoPath), 1},
    {offsetof(DataInter, Left_ID), sizeof(((DataInter *)0)->Left_ID), 0},
    {offsetof(DataInter, Right_ID), sizeof(((DataInter *)0)->Right_ID), 0},
    {offsetof(DataInter, DBTable), sizeof(((DataInter *)0)->DBTable), 1},
    {offsetof(DataInter, ValidFrom), sizeof(((DataInter *)0)->ValidFrom), 1},
    {offsetof(DataInter, ValidTo), sizeof(((DataInter *)0)->ValidTo), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataInter, Name), sizeof(((DataInter *)0)->Name), 1},
    {offsetof(DataInter, Abbrev), sizeof(((DataInter *)0)->Abbrev), 1},
    {offsetof(DataInter, Web), sizeof(((DataInter *)0)->Web), 1},
    {offsetof(DataInter, LogoPath), sizeof(((DataInter *)0)->LogoPath), 1},
    {offsetof(DataInter, Left_ID), sizeof(((DataInter *)0)->Left_ID), 0},
    {offsetof(DataInter, Right_ID), sizeof(((DataInter *)0)->Right_ID), 0},
    {offsetof(DataInter, DBTable), sizeof(((DataInter *)0)->DBTable), 1},
    {offsetof(DataInter, ValidFrom), sizeof(((DataInter *)0)->ValidFrom), 1},
    {offsetof(DataInter, ValidTo), sizeof(((DataInter *)0)->ValidTo), 1},
};

static int LoadInterRows(MYSQL *db, DataInter **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataInter_ID`, `DataInter_Name`, `DataInter_Abbrev`, `DataInter_Web`, `DataInter_LogoPath`, `DataInter_Left_ID`, `DataInter_Right_ID`, `DataInter_DBTable`, `DataInter_ValidFrom`, `DataInter_ValidTo` FROM `DataInter`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataInter), offsetof(DataInter, ID), offsetof(DataInter, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadInterIntoMemory3(MYSQL *db, DataInter **records, size_t *count) { return LoadInterRows(db, records, count, 0); }
int LoadInterIntoMemory4(MYSQL *db, DataInter **records, size_t *count, size_t limit) { return LoadInterRows(db, records, count, limit); }

void PrintInter(const DataInter *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" Name=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->Name);
    printf(" Abbrev=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->Abbrev);
    printf(" Web=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->Web);
    printf(" LogoPath=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->LogoPath);
    printf(" Left_ID=%" PRIu32, record->Left_ID);
    printf(" Right_ID=%" PRIu32, record->Right_ID);
    printf(" DBTable=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->DBTable);
    printf(" ValidFrom=%s", (record->nullFields & (UINT32_C(1) << 8)) ? "<NULL>" : record->ValidFrom);
    printf(" ValidTo=%s", (record->nullFields & (UINT32_C(1) << 9)) ? "<NULL>" : record->ValidTo);
    putchar('\n'); }

int ParseInterQuery(char *line, void *opaqueRecord) {
    DataInter *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->Name) { fprintf(stderr, "Name exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Name, token0, len0 + 1);
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->Abbrev) { fprintf(stderr, "Abbrev exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Abbrev, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->Web) { fprintf(stderr, "Web exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Web, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->LogoPath) { fprintf(stderr, "LogoPath exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->LogoPath, token3, len3 + 1);
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        errno = 0; char *end4 = NULL; uintmax_t value4 = strtoumax(token4, &end4, 10); if (errno || end4 == token4 || *end4 || value4 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Left_ID: %s\n", token4); return EXIT_FAILURE; } record->Left_ID = (uint32_t)value4;
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        errno = 0; char *end5 = NULL; uintmax_t value5 = strtoumax(token5, &end5, 10); if (errno || end5 == token5 || *end5 || value5 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Right_ID: %s\n", token5); return EXIT_FAILURE; } record->Right_ID = (uint32_t)value5;
    }
    char *token6 = p; char *comma6 = strchr(p, ','); if (!comma6) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma6 = '\0'; p = comma6 + 1;
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->DBTable) { fprintf(stderr, "DBTable exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DBTable, token6, len6 + 1);
    }
    char *token7 = p; char *comma7 = strchr(p, ','); if (!comma7) { fprintf(stderr, "Too few comma-separated fields for Inter\n"); return EXIT_FAILURE; } *comma7 = '\0'; p = comma7 + 1;
    if (*token7 == '\0') { nulls |= UINT32_C(1) << 8; } else {
        size_t len7 = strlen(token7); if (len7 >= sizeof record->ValidFrom) { fprintf(stderr, "ValidFrom exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ValidFrom, token7, len7 + 1);
    }
    char *token8 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Inter\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token8 == '\0') { nulls |= UINT32_C(1) << 9; } else {
        size_t len8 = strlen(token8); if (len8 >= sizeof record->ValidTo) { fprintf(stderr, "ValidTo exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ValidTo, token8, len8 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Inter\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseInterRecord(char *line, void *record) { return ParseInterQuery(line, record); }

InterHashTable *BuildInterHashTable(const DataInter *records, size_t count) {
    InterHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataInter, ID), offsetof(DataInter, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
InterMatch FindInterID(const InterHashTable *table, const DataInter *record) { InterMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeInterMatch(InterMatch *match) { FreeRecordMatch(match); }
void FreeInterHashTable(InterHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryInterMemory(const void *opaque, const void *record) {
    const InterHashTable *table = opaque; const DataInter *query = record; if (!table || !query) return EXIT_FAILURE; InterMatch m = FindInterID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeInterMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryInterDatabase(MYSQL *db, const void *record) {
    const DataInter *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataInter_ID` FROM `DataInter` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataInter_Name` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_Name` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Name); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Name,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataInter_Abbrev` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_Abbrev` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Abbrev); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Abbrev,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataInter_Web` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_Web` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Web); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Web,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataInter_LogoPath` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_LogoPath` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->LogoPath); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->LogoPath,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataInter_Left_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataInter_Left_ID` = %" PRIu32,q->Left_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataInter_Right_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataInter_Right_ID` = %" PRIu32,q->Right_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`DataInter_DBTable` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_DBTable` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DBTable); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DBTable,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (7 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 8)) { const char *part = "`DataInter_ValidFrom` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_ValidFrom` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ValidFrom); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ValidFrom,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (8 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 9)) { const char *part = "`DataInter_ValidTo` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataInter_ValidTo` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ValidTo); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ValidTo,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
