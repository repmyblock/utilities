#include "VoterComplementInfo.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VoterComplementInfoHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(VoterComplementInfo, Voters_ID), sizeof(((VoterComplementInfo *)0)->Voters_ID), 0},
    {offsetof(VoterComplementInfo, PrevName), sizeof(((VoterComplementInfo *)0)->PrevName), 1},
    {offsetof(VoterComplementInfo, PrevAddress), sizeof(((VoterComplementInfo *)0)->PrevAddress), 1},
    {offsetof(VoterComplementInfo, DataCountyID_PrevCounty), sizeof(((VoterComplementInfo *)0)->DataCountyID_PrevCounty), 0},
    {offsetof(VoterComplementInfo, LastYearVoted), sizeof(((VoterComplementInfo *)0)->LastYearVoted), 1},
    {offsetof(VoterComplementInfo, LastDateVoted), sizeof(((VoterComplementInfo *)0)->LastDateVoted), 1},
    {offsetof(VoterComplementInfo, OtherParty), sizeof(((VoterComplementInfo *)0)->OtherParty), 1},
};
static const RecordField keyFields[] = {
    {offsetof(VoterComplementInfo, Voters_ID), sizeof(((VoterComplementInfo *)0)->Voters_ID), 0},
    {offsetof(VoterComplementInfo, PrevName), sizeof(((VoterComplementInfo *)0)->PrevName), 1},
    {offsetof(VoterComplementInfo, PrevAddress), sizeof(((VoterComplementInfo *)0)->PrevAddress), 1},
    {offsetof(VoterComplementInfo, DataCountyID_PrevCounty), sizeof(((VoterComplementInfo *)0)->DataCountyID_PrevCounty), 0},
    {offsetof(VoterComplementInfo, LastYearVoted), sizeof(((VoterComplementInfo *)0)->LastYearVoted), 1},
    {offsetof(VoterComplementInfo, LastDateVoted), sizeof(((VoterComplementInfo *)0)->LastDateVoted), 1},
    {offsetof(VoterComplementInfo, OtherParty), sizeof(((VoterComplementInfo *)0)->OtherParty), 1},
};

static int LoadVoterComplementInfoRows(MYSQL *db, VoterComplementInfo **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `VotersComplementInfo_ID`, `Voters_ID`, `VotersComplementInfo_PrevName`, `VotersComplementInfo_PrevAddress`, `DataCountyID_PrevCounty`, `VotersComplementInfo_LastYearVoted`, `VotersComplementInfo_LastDateVoted`, `VotersComplementInfo_OtherParty` FROM `VotersComplementInfo`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(VoterComplementInfo), offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadVoterComplementInfoIntoMemory3(MYSQL *db, VoterComplementInfo **records, size_t *count) { return LoadVoterComplementInfoRows(db, records, count, 0); }
int LoadVoterComplementInfoIntoMemory4(MYSQL *db, VoterComplementInfo **records, size_t *count, size_t limit) { return LoadVoterComplementInfoRows(db, records, count, limit); }

void PrintVoterComplementInfo(const VoterComplementInfo *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" Voters_ID=%" PRIu32, record->Voters_ID);
    printf(" PrevName=%s", (record->nullFields & (UINT32_C(1) << 2)) ? "<NULL>" : record->PrevName);
    printf(" PrevAddress=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->PrevAddress);
    printf(" DataCountyID_PrevCounty=%" PRIu32, record->DataCountyID_PrevCounty);
    printf(" LastYearVoted=%s", (record->nullFields & (UINT32_C(1) << 5)) ? "<NULL>" : record->LastYearVoted);
    printf(" LastDateVoted=%s", (record->nullFields & (UINT32_C(1) << 6)) ? "<NULL>" : record->LastDateVoted);
    printf(" OtherParty=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->OtherParty);
    putchar('\n'); }

int ParseVoterComplementInfoQuery(char *line, void *opaqueRecord) {
    VoterComplementInfo *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for Voters_ID: %s\n", token0); return EXIT_FAILURE; } record->Voters_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        size_t len1 = strlen(token1); if (len1 >= sizeof record->PrevName) { fprintf(stderr, "PrevName exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->PrevName, token1, len1 + 1);
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->PrevAddress) { fprintf(stderr, "PrevAddress exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->PrevAddress, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        errno = 0; char *end3 = NULL; uintmax_t value3 = strtoumax(token3, &end3, 10); if (errno || end3 == token3 || *end3 || value3 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataCountyID_PrevCounty: %s\n", token3); return EXIT_FAILURE; } record->DataCountyID_PrevCounty = (uint32_t)value3;
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        size_t len4 = strlen(token4); if (len4 >= sizeof record->LastYearVoted) { fprintf(stderr, "LastYearVoted exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->LastYearVoted, token4, len4 + 1);
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        size_t len5 = strlen(token5); if (len5 >= sizeof record->LastDateVoted) { fprintf(stderr, "LastDateVoted exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->LastDateVoted, token5, len5 + 1);
    }
    char *token6 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->OtherParty) { fprintf(stderr, "OtherParty exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->OtherParty, token6, len6 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for VoterComplementInfo\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseVoterComplementInfoRecord(char *line, void *record) { return ParseVoterComplementInfoQuery(line, record); }

VoterComplementInfoHashTable *BuildVoterComplementInfoHashTable(const VoterComplementInfo *records, size_t count) {
    VoterComplementInfoHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(VoterComplementInfo, ID), offsetof(VoterComplementInfo, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
VoterComplementInfoMatch FindVoterComplementInfoID(const VoterComplementInfoHashTable *table, const VoterComplementInfo *record) { VoterComplementInfoMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeVoterComplementInfoMatch(VoterComplementInfoMatch *match) { FreeRecordMatch(match); }
void FreeVoterComplementInfoHashTable(VoterComplementInfoHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryVoterComplementInfoMemory(const void *opaque, const void *record) {
    const VoterComplementInfoHashTable *table = opaque; const VoterComplementInfo *query = record; if (!table || !query) return EXIT_FAILURE; VoterComplementInfoMatch m = FindVoterComplementInfoID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeVoterComplementInfoMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryVoterComplementInfoDatabase(MYSQL *db, const void *record) {
    const VoterComplementInfo *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `VotersComplementInfo_ID` FROM `VotersComplementInfo` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`Voters_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`Voters_ID` = %" PRIu32,q->Voters_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`VotersComplementInfo_PrevName` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersComplementInfo_PrevName` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->PrevName); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->PrevName,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`VotersComplementInfo_PrevAddress` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersComplementInfo_PrevAddress` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->PrevAddress); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->PrevAddress,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`DataCountyID_PrevCounty` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataCountyID_PrevCounty` = %" PRIu32,q->DataCountyID_PrevCounty); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`VotersComplementInfo_LastYearVoted` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersComplementInfo_LastYearVoted` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->LastYearVoted); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->LastYearVoted,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`VotersComplementInfo_LastDateVoted` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersComplementInfo_LastDateVoted` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->LastDateVoted); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->LastDateVoted,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`VotersComplementInfo_OtherParty` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`VotersComplementInfo_OtherParty` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->OtherParty); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->OtherParty,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
