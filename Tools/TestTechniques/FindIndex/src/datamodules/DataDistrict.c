#include "DataDistrict.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DistrictHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataDistrict, ZipCode), sizeof(((DataDistrict *)0)->ZipCode), 1},
    {offsetof(DataDistrict, DBTable), sizeof(((DataDistrict *)0)->DBTable), 1},
    {offsetof(DataDistrict, DBTableValue), sizeof(((DataDistrict *)0)->DBTableValue), 1},
    {offsetof(DataDistrict, DataCounty_ID), sizeof(((DataDistrict *)0)->DataCounty_ID), 0},
    {offsetof(DataDistrict, DataDistrictTown_ID), sizeof(((DataDistrict *)0)->DataDistrictTown_ID), 0},
    {offsetof(DataDistrict, Electoral), sizeof(((DataDistrict *)0)->Electoral), 0},
    {offsetof(DataDistrict, StateAssembly), sizeof(((DataDistrict *)0)->StateAssembly), 0},
    {offsetof(DataDistrict, StateSenate), sizeof(((DataDistrict *)0)->StateSenate), 0},
    {offsetof(DataDistrict, Legislative), sizeof(((DataDistrict *)0)->Legislative), 0},
    {offsetof(DataDistrict, Ward), sizeof(((DataDistrict *)0)->Ward), 1},
    {offsetof(DataDistrict, Congress), sizeof(((DataDistrict *)0)->Congress), 0},
};
static const RecordField keyFields[] = {
    {offsetof(DataDistrict, ZipCode), sizeof(((DataDistrict *)0)->ZipCode), 1},
    {offsetof(DataDistrict, DBTable), sizeof(((DataDistrict *)0)->DBTable), 1},
    {offsetof(DataDistrict, DBTableValue), sizeof(((DataDistrict *)0)->DBTableValue), 1},
    {offsetof(DataDistrict, DataCounty_ID), sizeof(((DataDistrict *)0)->DataCounty_ID), 0},
    {offsetof(DataDistrict, DataDistrictTown_ID), sizeof(((DataDistrict *)0)->DataDistrictTown_ID), 0},
    {offsetof(DataDistrict, Electoral), sizeof(((DataDistrict *)0)->Electoral), 0},
    {offsetof(DataDistrict, StateAssembly), sizeof(((DataDistrict *)0)->StateAssembly), 0},
    {offsetof(DataDistrict, StateSenate), sizeof(((DataDistrict *)0)->StateSenate), 0},
    {offsetof(DataDistrict, Legislative), sizeof(((DataDistrict *)0)->Legislative), 0},
    {offsetof(DataDistrict, Ward), sizeof(((DataDistrict *)0)->Ward), 1},
    {offsetof(DataDistrict, Congress), sizeof(((DataDistrict *)0)->Congress), 0},
};

static int LoadDistrictRows(MYSQL *db, DataDistrict **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataDistrict_ID`, `DataDistrict_ZipCode`, `DataDistrict_DBTable`, `DataDistrict_DBTableValue`, `DataCounty_ID`, `DataDistrictTown_ID`, `DataDistrict_Electoral`, `DataDistrict_StateAssembly`, `DataDistrict_StateSenate`, `DataDistrict_Legislative`, `DataDistrict_Ward`, `DataDistrict_Congress` FROM `DataDistrict`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataDistrict), offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadDistrictIntoMemory3(MYSQL *db, DataDistrict **records, size_t *count) { return LoadDistrictRows(db, records, count, 0); }
int LoadDistrictIntoMemory4(MYSQL *db, DataDistrict **records, size_t *count, size_t limit) { return LoadDistrictRows(db, records, count, limit); }

void PrintDistrict(const DataDistrict *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" ZipCode=%s", (record->nullFields & (UINT32_C(1) << 1)) ? "<NULL>" : record->ZipCode);
    printf(" DBTable=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->DBTable);
    printf(" DBTableValue=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->DBTableValue);
    printf(" DataCounty_ID=%" PRIu32, record->DataCounty_ID);
    printf(" DataDistrictTown_ID=%" PRIu32, record->DataDistrictTown_ID);
    printf(" Electoral=%" PRIu32, record->Electoral);
    printf(" StateAssembly=%" PRIu32, record->StateAssembly);
    printf(" StateSenate=%" PRIu32, record->StateSenate);
    printf(" Legislative=%" PRIu32, record->Legislative);
    printf(" Ward=%s", (record->nullFields & (UINT32_C(1) << 10)) ? "<NULL>" : record->Ward);
    printf(" Congress=%" PRIu32, record->Congress);
    putchar('\n'); }

int ParseDistrictQuery(char *line, void *opaqueRecord) {
    DataDistrict *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        size_t len0 = strlen(token0); if (len0 >= sizeof record->ZipCode) { fprintf(stderr, "ZipCode exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ZipCode, token0, len0 + 1);
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->DBTable) { fprintf(stderr, "DBTable exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DBTable, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->DBTableValue) { fprintf(stderr, "DBTableValue exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DBTableValue, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        errno = 0; char *end3 = NULL; uintmax_t value3 = strtoumax(token3, &end3, 10); if (errno || end3 == token3 || *end3 || value3 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataCounty_ID: %s\n", token3); return EXIT_FAILURE; } record->DataCounty_ID = (uint32_t)value3;
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        errno = 0; char *end4 = NULL; uintmax_t value4 = strtoumax(token4, &end4, 10); if (errno || end4 == token4 || *end4 || value4 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataDistrictTown_ID: %s\n", token4); return EXIT_FAILURE; } record->DataDistrictTown_ID = (uint32_t)value4;
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        errno = 0; char *end5 = NULL; uintmax_t value5 = strtoumax(token5, &end5, 10); if (errno || end5 == token5 || *end5 || value5 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Electoral: %s\n", token5); return EXIT_FAILURE; } record->Electoral = (uint32_t)value5;
    }
    char *token6 = p; char *comma6 = strchr(p, ','); if (!comma6) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma6 = '\0'; p = comma6 + 1;
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        errno = 0; char *end6 = NULL; uintmax_t value6 = strtoumax(token6, &end6, 10); if (errno || end6 == token6 || *end6 || value6 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for StateAssembly: %s\n", token6); return EXIT_FAILURE; } record->StateAssembly = (uint32_t)value6;
    }
    char *token7 = p; char *comma7 = strchr(p, ','); if (!comma7) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma7 = '\0'; p = comma7 + 1;
    if (*token7 == '\0') { nulls |= UINT32_C(1) << 8; } else {
        errno = 0; char *end7 = NULL; uintmax_t value7 = strtoumax(token7, &end7, 10); if (errno || end7 == token7 || *end7 || value7 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for StateSenate: %s\n", token7); return EXIT_FAILURE; } record->StateSenate = (uint32_t)value7;
    }
    char *token8 = p; char *comma8 = strchr(p, ','); if (!comma8) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma8 = '\0'; p = comma8 + 1;
    if (*token8 == '\0') { nulls |= UINT32_C(1) << 9; } else {
        errno = 0; char *end8 = NULL; uintmax_t value8 = strtoumax(token8, &end8, 10); if (errno || end8 == token8 || *end8 || value8 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Legislative: %s\n", token8); return EXIT_FAILURE; } record->Legislative = (uint32_t)value8;
    }
    char *token9 = p; char *comma9 = strchr(p, ','); if (!comma9) { fprintf(stderr, "Too few comma-separated fields for District\n"); return EXIT_FAILURE; } *comma9 = '\0'; p = comma9 + 1;
    if (*token9 == '\0') { nulls |= UINT32_C(1) << 10; } else {
        size_t len9 = strlen(token9); if (len9 >= sizeof record->Ward) { fprintf(stderr, "Ward exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Ward, token9, len9 + 1);
    }
    char *token10 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for District\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token10 == '\0') { nulls |= UINT32_C(1) << 11; } else {
        errno = 0; char *end10 = NULL; uintmax_t value10 = strtoumax(token10, &end10, 10); if (errno || end10 == token10 || *end10 || value10 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Congress: %s\n", token10); return EXIT_FAILURE; } record->Congress = (uint32_t)value10;
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for District\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseDistrictRecord(char *line, void *record) { return ParseDistrictQuery(line, record); }

DistrictHashTable *BuildDistrictHashTable(const DataDistrict *records, size_t count) {
    DistrictHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataDistrict, ID), offsetof(DataDistrict, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
DistrictMatch FindDistrictID(const DistrictHashTable *table, const DataDistrict *record) { DistrictMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeDistrictMatch(DistrictMatch *match) { FreeRecordMatch(match); }
void FreeDistrictHashTable(DistrictHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryDistrictMemory(const void *opaque, const void *record) {
    const DistrictHashTable *table = opaque; const DataDistrict *query = record; if (!table || !query) return EXIT_FAILURE; DistrictMatch m = FindDistrictID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeDistrictMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryDistrictDatabase(MYSQL *db, const void *record) {
    const DataDistrict *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataDistrict_ID` FROM `DataDistrict` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataDistrict_ZipCode` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataDistrict_ZipCode` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ZipCode); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ZipCode,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataDistrict_DBTable` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataDistrict_DBTable` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DBTable); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DBTable,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataDistrict_DBTableValue` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataDistrict_DBTableValue` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DBTableValue); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DBTableValue,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataCounty_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCounty_ID` = %" PRIu32,q->DataCounty_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataDistrictTown_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrictTown_ID` = %" PRIu32,q->DataDistrictTown_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataDistrict_Electoral` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrict_Electoral` = %" PRIu32,q->Electoral); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`DataDistrict_StateAssembly` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrict_StateAssembly` = %" PRIu32,q->StateAssembly); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (7 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 8)) { const char *part = "`DataDistrict_StateSenate` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrict_StateSenate` = %" PRIu32,q->StateSenate); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (8 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 9)) { const char *part = "`DataDistrict_Legislative` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrict_Legislative` = %" PRIu32,q->Legislative); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (9 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 10)) { const char *part = "`DataDistrict_Ward` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataDistrict_Ward` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Ward); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Ward,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (10 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 11)) { const char *part = "`DataDistrict_Congress` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataDistrict_Congress` = %" PRIu32,q->Congress); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
