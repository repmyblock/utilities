#include "DataClub.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ClubHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(DataClub, State_ID), sizeof(((DataClub *)0)->State_ID), 0},
    {offsetof(DataClub, DataCounty_ID), sizeof(((DataClub *)0)->DataCounty_ID), 0},
    {offsetof(DataClub, DataParty_ID), sizeof(((DataClub *)0)->DataParty_ID), 0},
    {offsetof(DataClub, Name), sizeof(((DataClub *)0)->Name), 1},
    {offsetof(DataClub, Web), sizeof(((DataClub *)0)->Web), 1},
    {offsetof(DataClub, Email), sizeof(((DataClub *)0)->Email), 1},
    {offsetof(DataClub, enum_field), sizeof(((DataClub *)0)->enum_field), 1},
    {offsetof(DataClub, LastVerified), sizeof(((DataClub *)0)->LastVerified), 1},
};
static const RecordField keyFields[] = {
    {offsetof(DataClub, State_ID), sizeof(((DataClub *)0)->State_ID), 0},
    {offsetof(DataClub, DataCounty_ID), sizeof(((DataClub *)0)->DataCounty_ID), 0},
    {offsetof(DataClub, DataParty_ID), sizeof(((DataClub *)0)->DataParty_ID), 0},
    {offsetof(DataClub, Name), sizeof(((DataClub *)0)->Name), 1},
    {offsetof(DataClub, Web), sizeof(((DataClub *)0)->Web), 1},
    {offsetof(DataClub, Email), sizeof(((DataClub *)0)->Email), 1},
    {offsetof(DataClub, enum_field), sizeof(((DataClub *)0)->enum_field), 1},
    {offsetof(DataClub, LastVerified), sizeof(((DataClub *)0)->LastVerified), 1},
};

static int LoadClubRows(MYSQL *db, DataClub **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `DataClub_ID`, `State_ID`, `DataCounty_ID`, `DataParty_ID`, `DataClub_Name`, `DataClub_Web`, `DataClub_Email`, `DataClub_enum`, `DataClub_LastVerified` FROM `DataClub`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(DataClub), offsetof(DataClub, ID), offsetof(DataClub, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadClubIntoMemory3(MYSQL *db, DataClub **records, size_t *count) { return LoadClubRows(db, records, count, 0); }
int LoadClubIntoMemory4(MYSQL *db, DataClub **records, size_t *count, size_t limit) { return LoadClubRows(db, records, count, limit); }

void PrintClub(const DataClub *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" State_ID=%" PRIu32, record->State_ID);
    printf(" DataCounty_ID=%" PRIu32, record->DataCounty_ID);
    printf(" DataParty_ID=%" PRIu32, record->DataParty_ID);
    printf(" Name=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->Name);
    printf(" Web=%s", (record->nullFields & (UINT32_C(1) << 5)) ? "<NULL>" : record->Web);
    printf(" Email=%s", (record->nullFields & (UINT32_C(1) << 6)) ? "<NULL>" : record->Email);
    printf(" enum_field=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->enum_field);
    printf(" LastVerified=%s", (record->nullFields & (UINT32_C(1) << 8)) ? "<NULL>" : record->LastVerified);
    putchar('\n'); }

int ParseClubQuery(char *line, void *opaqueRecord) {
    DataClub *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for State_ID: %s\n", token0); return EXIT_FAILURE; } record->State_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        errno = 0; char *end1 = NULL; uintmax_t value1 = strtoumax(token1, &end1, 10); if (errno || end1 == token1 || *end1 || value1 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataCounty_ID: %s\n", token1); return EXIT_FAILURE; } record->DataCounty_ID = (uint32_t)value1;
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        errno = 0; char *end2 = NULL; uintmax_t value2 = strtoumax(token2, &end2, 10); if (errno || end2 == token2 || *end2 || value2 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataParty_ID: %s\n", token2); return EXIT_FAILURE; } record->DataParty_ID = (uint32_t)value2;
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->Name) { fprintf(stderr, "Name exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Name, token3, len3 + 1);
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        size_t len4 = strlen(token4); if (len4 >= sizeof record->Web) { fprintf(stderr, "Web exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Web, token4, len4 + 1);
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        size_t len5 = strlen(token5); if (len5 >= sizeof record->Email) { fprintf(stderr, "Email exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Email, token5, len5 + 1);
    }
    char *token6 = p; char *comma6 = strchr(p, ','); if (!comma6) { fprintf(stderr, "Too few comma-separated fields for Club\n"); return EXIT_FAILURE; } *comma6 = '\0'; p = comma6 + 1;
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->enum_field) { fprintf(stderr, "enum_field exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->enum_field, token6, len6 + 1);
    }
    char *token7 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Club\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token7 == '\0') { nulls |= UINT32_C(1) << 8; } else {
        size_t len7 = strlen(token7); if (len7 >= sizeof record->LastVerified) { fprintf(stderr, "LastVerified exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->LastVerified, token7, len7 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Club\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseClubRecord(char *line, void *record) { return ParseClubQuery(line, record); }

ClubHashTable *BuildClubHashTable(const DataClub *records, size_t count) {
    ClubHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(DataClub, ID), offsetof(DataClub, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
ClubMatch FindClubID(const ClubHashTable *table, const DataClub *record) { ClubMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeClubMatch(ClubMatch *match) { FreeRecordMatch(match); }
void FreeClubHashTable(ClubHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryClubMemory(const void *opaque, const void *record) {
    const ClubHashTable *table = opaque; const DataClub *query = record; if (!table || !query) return EXIT_FAILURE; ClubMatch m = FindClubID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeClubMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryClubDatabase(MYSQL *db, const void *record) {
    const DataClub *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `DataClub_ID` FROM `DataClub` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`State_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`State_ID` = %" PRIu32,q->State_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataCounty_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCounty_ID` = %" PRIu32,q->DataCounty_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataParty_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataParty_ID` = %" PRIu32,q->DataParty_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataClub_Name` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataClub_Name` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Name); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Name,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataClub_Web` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataClub_Web` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Web); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Web,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataClub_Email` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataClub_Email` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Email); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Email,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`DataClub_enum` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataClub_enum` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->enum_field); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->enum_field,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (7 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 8)) { const char *part = "`DataClub_LastVerified` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`DataClub_LastVerified` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->LastVerified); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->LastVerified,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
