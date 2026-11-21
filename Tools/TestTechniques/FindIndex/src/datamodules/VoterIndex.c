#include "VoterIndex.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VoterIndexHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(VoterIndex, DataLastName_ID), sizeof(((VoterIndex *)0)->DataLastName_ID), 0},
    {offsetof(VoterIndex, DataFirstName_ID), sizeof(((VoterIndex *)0)->DataFirstName_ID), 0},
    {offsetof(VoterIndex, DataMiddleName_ID), sizeof(((VoterIndex *)0)->DataMiddleName_ID), 0},
    {offsetof(VoterIndex, Suffix), sizeof(((VoterIndex *)0)->Suffix), 1},
    {offsetof(VoterIndex, DOB), sizeof(((VoterIndex *)0)->DOB), 1},
    {offsetof(VoterIndex, DataState_ID), sizeof(((VoterIndex *)0)->DataState_ID), 0},
    {offsetof(VoterIndex, UniqStateVoterID), sizeof(((VoterIndex *)0)->UniqStateVoterID), 1},
};
static const RecordField keyFields[] = {
    {offsetof(VoterIndex, DataLastName_ID), sizeof(((VoterIndex *)0)->DataLastName_ID), 0},
    {offsetof(VoterIndex, DataFirstName_ID), sizeof(((VoterIndex *)0)->DataFirstName_ID), 0},
    {offsetof(VoterIndex, DataMiddleName_ID), sizeof(((VoterIndex *)0)->DataMiddleName_ID), 0},
    {offsetof(VoterIndex, Suffix), sizeof(((VoterIndex *)0)->Suffix), 1},
    {offsetof(VoterIndex, DOB), sizeof(((VoterIndex *)0)->DOB), 1},
    {offsetof(VoterIndex, DataState_ID), sizeof(((VoterIndex *)0)->DataState_ID), 0},
    {offsetof(VoterIndex, UniqStateVoterID), sizeof(((VoterIndex *)0)->UniqStateVoterID), 1},
};

static int LoadVoterIndexRows(MYSQL *db, VoterIndex **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `VotersIndexes_ID`, `DataLastName_ID`, `DataFirstName_ID`, `DataMiddleName_ID`, `VotersIndexes_Suffix`, `VotersIndexes_DOB`, `DataState_ID`, `VotersIndexes_UniqStateVoterID` FROM `VotersIndexes`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(VoterIndex), offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadVoterIndexIntoMemory3(MYSQL *db, VoterIndex **records, size_t *count) { return LoadVoterIndexRows(db, records, count, 0); }
int LoadVoterIndexIntoMemory4(MYSQL *db, VoterIndex **records, size_t *count, size_t limit) { return LoadVoterIndexRows(db, records, count, limit); }

void PrintVoterIndex(const VoterIndex *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" DataLastName_ID=%" PRIu32, record->DataLastName_ID);
    printf(" DataFirstName_ID=%" PRIu32, record->DataFirstName_ID);
    printf(" DataMiddleName_ID=%" PRIu32, record->DataMiddleName_ID);
    printf(" Suffix=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->Suffix);
    printf(" DOB=%s", (record->nullFields & (UINT32_C(1) << 5)) ? "<NULL>" : record->DOB);
    printf(" DataState_ID=%" PRIu32, record->DataState_ID);
    printf(" UniqStateVoterID=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->UniqStateVoterID);
    putchar('\n'); }

int ParseVoterIndexQuery(char *line, void *opaqueRecord) {
    VoterIndex *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataLastName_ID: %s\n", token0); return EXIT_FAILURE; } record->DataLastName_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        errno = 0; char *end1 = NULL; uintmax_t value1 = strtoumax(token1, &end1, 10); if (errno || end1 == token1 || *end1 || value1 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataFirstName_ID: %s\n", token1); return EXIT_FAILURE; } record->DataFirstName_ID = (uint32_t)value1;
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        errno = 0; char *end2 = NULL; uintmax_t value2 = strtoumax(token2, &end2, 10); if (errno || end2 == token2 || *end2 || value2 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataMiddleName_ID: %s\n", token2); return EXIT_FAILURE; } record->DataMiddleName_ID = (uint32_t)value2;
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->Suffix) { fprintf(stderr, "Suffix exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Suffix, token3, len3 + 1);
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        size_t len4 = strlen(token4); if (len4 >= sizeof record->DOB) { fprintf(stderr, "DOB exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DOB, token4, len4 + 1);
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        errno = 0; char *end5 = NULL; uintmax_t value5 = strtoumax(token5, &end5, 10); if (errno || end5 == token5 || *end5 || value5 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataState_ID: %s\n", token5); return EXIT_FAILURE; } record->DataState_ID = (uint32_t)value5;
    }
    char *token6 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->UniqStateVoterID) { fprintf(stderr, "UniqStateVoterID exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->UniqStateVoterID, token6, len6 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for VoterIndex\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseVoterIndexRecord(char *line, void *record) { return ParseVoterIndexQuery(line, record); }

VoterIndexHashTable *BuildVoterIndexHashTable(const VoterIndex *records, size_t count) {
    VoterIndexHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(VoterIndex, ID), offsetof(VoterIndex, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
VoterIndexMatch FindVoterIndexID(const VoterIndexHashTable *table, const VoterIndex *record) { VoterIndexMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeVoterIndexMatch(VoterIndexMatch *match) { FreeRecordMatch(match); }
void FreeVoterIndexHashTable(VoterIndexHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryVoterIndexMemory(const void *opaque, const void *record) {
    const VoterIndexHashTable *table = opaque; const VoterIndex *query = record; if (!table || !query) return EXIT_FAILURE; VoterIndexMatch m = FindVoterIndexID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeVoterIndexMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryVoterIndexDatabase(MYSQL *db, const void *record) {
    const VoterIndex *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `VotersIndexes_ID` FROM `VotersIndexes` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`DataLastName_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataLastName_ID` = %" PRIu32,q->DataLastName_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataFirstName_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataFirstName_ID` = %" PRIu32,q->DataFirstName_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`DataMiddleName_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataMiddleName_ID` = %" PRIu32,q->DataMiddleName_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`VotersIndexes_Suffix` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersIndexes_Suffix` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Suffix); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Suffix,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`VotersIndexes_DOB` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersIndexes_DOB` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DOB); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DOB,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`DataState_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataState_ID` = %" PRIu32,q->DataState_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`VotersIndexes_UniqStateVoterID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersIndexes_UniqStateVoterID` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->UniqStateVoterID); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->UniqStateVoterID,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
