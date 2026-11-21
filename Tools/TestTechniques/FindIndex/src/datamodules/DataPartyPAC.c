#include "DataPartyPAC.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PartyPACHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataPartyPAC, DataPAC_ID), sizeof(((DataPartyPAC *)0)->DataPAC_ID), 0},
    {offsetof(DataPartyPAC, DataParty_ID), sizeof(((DataPartyPAC *)0)->DataParty_ID), 0},
    {offsetof(DataPartyPAC, ValidFrom), sizeof(((DataPartyPAC *)0)->ValidFrom), 1},
    {offsetof(DataPartyPAC, ValidTo), sizeof(((DataPartyPAC *)0)->ValidTo), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataPartyPAC, DataPAC_ID), sizeof(((DataPartyPAC *)0)->DataPAC_ID), 0},
    {offsetof(DataPartyPAC, DataParty_ID), sizeof(((DataPartyPAC *)0)->DataParty_ID), 0},
    {offsetof(DataPartyPAC, ValidFrom), sizeof(((DataPartyPAC *)0)->ValidFrom), 1},
    {offsetof(DataPartyPAC, ValidTo), sizeof(((DataPartyPAC *)0)->ValidTo), 1},
};

static int LoadPartyPACRows(MYSQL *db, DataPartyPAC **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataPartyPAC_ID`, `DataPAC_ID`, `DataParty_ID`, `DataPartyPAC_ValidFrom`, `DataPartyPAC_ValidTo` FROM `DataPartyPAC`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataPartyPAC), offsetof(DataPartyPAC, ID), offsetof(DataPartyPAC, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadPartyPACIntoMemory3(MYSQL *db, DataPartyPAC **records, size_t *count) { return LoadPartyPACRows(db, records, count, 0); }
int LoadPartyPACIntoMemory4(MYSQL *db, DataPartyPAC **records, size_t *count, size_t limit) { return LoadPartyPACRows(db, records, count, limit); }

void PrintPartyPAC(const DataPartyPAC *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" DataPAC_ID=%" PRIu32, record->DataPAC_ID);
    printf(" DataParty_ID=%" PRIu32, record->DataParty_ID);
    printf(" ValidFrom=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->ValidFrom);
    printf(" ValidTo=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->ValidTo);
    putchar('\n'); }

int ParsePartyPACQuery(char *line, void *opaqueRecord) {
    DataPartyPAC *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for PartyPAC\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataPAC_ID: %s\n", token0); return EXIT_FAILURE; } record->DataPAC_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for PartyPAC\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        errno = 0; char *end1 = NULL; uintmax_t value1 = strtoumax(token1, &end1, 10); if (errno || end1 == token1 || *end1 || value1 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataParty_ID: %s\n", token1); return EXIT_FAILURE; } record->DataParty_ID = (uint32_t)value1;
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for PartyPAC\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->ValidFrom) { fprintf(stderr, "ValidFrom exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ValidFrom, token2, len2 + 1);
    }
    char *token3 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for PartyPAC\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->ValidTo) { fprintf(stderr, "ValidTo exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ValidTo, token3, len3 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for PartyPAC\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParsePartyPACRecord(char *line, void *record) { return ParsePartyPACQuery(line, record); }

PartyPACHashTable *BuildPartyPACHashTable(const DataPartyPAC *records, size_t count) {
    PartyPACHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataPartyPAC, ID), offsetof(DataPartyPAC, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
PartyPACMatch FindPartyPACID(const PartyPACHashTable *table, const DataPartyPAC *record) { PartyPACMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreePartyPACMatch(PartyPACMatch *match) { FreeRecordMatch(match); }
void FreePartyPACHashTable(PartyPACHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryPartyPACMemory(const void *opaque, const void *record) {
    const PartyPACHashTable *table = opaque; const DataPartyPAC *query = record; if (!table || !query) return EXIT_FAILURE; PartyPACMatch m = FindPartyPACID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreePartyPACMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryPartyPACDatabase(MYSQL *db, const void *record) {
    const DataPartyPAC *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataPartyPAC_ID` FROM `DataPartyPAC` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataPAC_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataPAC_ID` = %" PRIu32,q->DataPAC_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataParty_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataParty_ID` = %" PRIu32,q->DataParty_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataPartyPAC_ValidFrom` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPartyPAC_ValidFrom` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ValidFrom); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ValidFrom,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataPartyPAC_ValidTo` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataPartyPAC_ValidTo` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ValidTo); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ValidTo,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
