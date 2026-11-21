#include "DataDistrictTown.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DistrictTownHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataDistrictTown, Name), sizeof(((DataDistrictTown *)0)->Name), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataDistrictTown, Name), sizeof(((DataDistrictTown *)0)->Name), 1},
};

static int LoadDistrictTownRows(MYSQL *db, DataDistrictTown **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataDistrictTown_ID`, `DataDistrictTown_Name` FROM `DataDistrictTown`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataDistrictTown), offsetof(DataDistrictTown, ID), offsetof(DataDistrictTown, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadDistrictTownIntoMemory3(MYSQL *db, DataDistrictTown **records, size_t *count) { return LoadDistrictTownRows(db, records, count, 0); }
int LoadDistrictTownIntoMemory4(MYSQL *db, DataDistrictTown **records, size_t *count, size_t limit) { return LoadDistrictTownRows(db, records, count, limit); }

void PrintDistrictTown(const DataDistrictTown *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" Name=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->Name);
    putchar('\n'); }

int ParseDistrictTownQuery(char *line, void *opaqueRecord) {
    DataDistrictTown *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for DistrictTown\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->Name) { fprintf(stderr, "Name exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Name, token0, len0 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for DistrictTown\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseDistrictTownRecord(char *line, void *record) { return ParseDistrictTownQuery(line, record); }

DistrictTownHashTable *BuildDistrictTownHashTable(const DataDistrictTown *records, size_t count) {
    DistrictTownHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataDistrictTown, ID), offsetof(DataDistrictTown, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
DistrictTownMatch FindDistrictTownID(const DistrictTownHashTable *table, const DataDistrictTown *record) { DistrictTownMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeDistrictTownMatch(DistrictTownMatch *match) { FreeRecordMatch(match); }
void FreeDistrictTownHashTable(DistrictTownHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryDistrictTownMemory(const void *opaque, const void *record) {
    const DistrictTownHashTable *table = opaque; const DataDistrictTown *query = record; if (!table || !query) return EXIT_FAILURE; DistrictTownMatch m = FindDistrictTownID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeDistrictTownMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryDistrictTownDatabase(MYSQL *db, const void *record) {
    const DataDistrictTown *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataDistrictTown_ID` FROM `DataDistrictTown` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataDistrictTown_Name` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataDistrictTown_Name` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Name); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Name,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
