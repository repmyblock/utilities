#include "DataMailingAddress.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MailingAddressHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataMailingAddress, Line1), sizeof(((DataMailingAddress *)0)->Line1), 1},
    {offsetof(DataMailingAddress, Line2), sizeof(((DataMailingAddress *)0)->Line2), 1},
    {offsetof(DataMailingAddress, Line3), sizeof(((DataMailingAddress *)0)->Line3), 1},
    {offsetof(DataMailingAddress, Line4), sizeof(((DataMailingAddress *)0)->Line4), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataMailingAddress, Line1), sizeof(((DataMailingAddress *)0)->Line1), 1},
    {offsetof(DataMailingAddress, Line2), sizeof(((DataMailingAddress *)0)->Line2), 1},
    {offsetof(DataMailingAddress, Line3), sizeof(((DataMailingAddress *)0)->Line3), 1},
    {offsetof(DataMailingAddress, Line4), sizeof(((DataMailingAddress *)0)->Line4), 1},
};

static int LoadMailingAddressRows(MYSQL *db, DataMailingAddress **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataMailingAddress_ID`, `DataMailingAddress_Line1`, `DataMailingAddress_Line2`, `DataMailingAddress_Line3`, `DataMailingAddress_Line4` FROM `DataMailingAddress`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataMailingAddress), offsetof(DataMailingAddress, ID), offsetof(DataMailingAddress, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadMailingAddressIntoMemory3(MYSQL *db, DataMailingAddress **records, size_t *count) { return LoadMailingAddressRows(db, records, count, 0); }
int LoadMailingAddressIntoMemory4(MYSQL *db, DataMailingAddress **records, size_t *count, size_t limit) { return LoadMailingAddressRows(db, records, count, limit); }

void PrintMailingAddress(const DataMailingAddress *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" Line1=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->Line1);
    printf(" Line2=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->Line2);
    printf(" Line3=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->Line3);
    printf(" Line4=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->Line4);
    putchar('\n'); }

int ParseMailingAddressQuery(char *line, void *opaqueRecord) {
    DataMailingAddress *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for MailingAddress\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->Line1) { fprintf(stderr, "Line1 exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Line1, token0, len0 + 1);
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for MailingAddress\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->Line2) { fprintf(stderr, "Line2 exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Line2, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for MailingAddress\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->Line3) { fprintf(stderr, "Line3 exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Line3, token2, len2 + 1);
    }
    char *token3 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for MailingAddress\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->Line4) { fprintf(stderr, "Line4 exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Line4, token3, len3 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for MailingAddress\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseMailingAddressRecord(char *line, void *record) { return ParseMailingAddressQuery(line, record); }

MailingAddressHashTable *BuildMailingAddressHashTable(const DataMailingAddress *records, size_t count) {
    MailingAddressHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataMailingAddress, ID), offsetof(DataMailingAddress, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
MailingAddressMatch FindMailingAddressID(const MailingAddressHashTable *table, const DataMailingAddress *record) { MailingAddressMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeMailingAddressMatch(MailingAddressMatch *match) { FreeRecordMatch(match); }
void FreeMailingAddressHashTable(MailingAddressHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryMailingAddressMemory(const void *opaque, const void *record) {
    const MailingAddressHashTable *table = opaque; const DataMailingAddress *query = record; if (!table || !query) return EXIT_FAILURE; MailingAddressMatch m = FindMailingAddressID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeMailingAddressMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryMailingAddressDatabase(MYSQL *db, const void *record) {
    const DataMailingAddress *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataMailingAddress_ID` FROM `DataMailingAddress` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataMailingAddress_Line1` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataMailingAddress_Line1` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Line1); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Line1,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataMailingAddress_Line2` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataMailingAddress_Line2` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Line2); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Line2,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataMailingAddress_Line3` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataMailingAddress_Line3` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Line3); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Line3,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataMailingAddress_Line4` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataMailingAddress_Line4` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Line4); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Line4,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
