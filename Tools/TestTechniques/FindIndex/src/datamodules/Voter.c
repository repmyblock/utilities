#include "Voter.h"
#include "../RecordLoader.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VoterHashTable { RecordHashTable *records; };

static const RecordField loadFields[] = {
    {offsetof(Voter, VotersIndexes_ID), sizeof(((Voter *)0)->VotersIndexes_ID), 0},
    {offsetof(Voter, DataHouse_ID), sizeof(((Voter *)0)->DataHouse_ID), 0},
    {offsetof(Voter, Gender), sizeof(((Voter *)0)->Gender), 1},
    {offsetof(Voter, UniqStateVoterID), sizeof(((Voter *)0)->UniqStateVoterID), 1},
    {offsetof(Voter, DataState_ID), sizeof(((Voter *)0)->DataState_ID), 0},
    {offsetof(Voter, RegParty), sizeof(((Voter *)0)->RegParty), 1},
    {offsetof(Voter, ReasonCode), sizeof(((Voter *)0)->ReasonCode), 1},
    {offsetof(Voter, Status), sizeof(((Voter *)0)->Status), 1},
    {offsetof(Voter, VotersMailingAddress_ID), sizeof(((Voter *)0)->VotersMailingAddress_ID), 0},
    {offsetof(Voter, IDRequired), sizeof(((Voter *)0)->IDRequired), 1},
    {offsetof(Voter, IDMet), sizeof(((Voter *)0)->IDMet), 1},
    {offsetof(Voter, ApplyDate), sizeof(((Voter *)0)->ApplyDate), 1},
    {offsetof(Voter, RegSource), sizeof(((Voter *)0)->RegSource), 1},
    {offsetof(Voter, DateInactive), sizeof(((Voter *)0)->DateInactive), 1},
    {offsetof(Voter, DatePurged), sizeof(((Voter *)0)->DatePurged), 1},
    {offsetof(Voter, CountyVoterNumber), sizeof(((Voter *)0)->CountyVoterNumber), 1},
    {offsetof(Voter, RMBActive), sizeof(((Voter *)0)->RMBActive), 1},
    {offsetof(Voter, RecFirstSeen), sizeof(((Voter *)0)->RecFirstSeen), 1},
    {offsetof(Voter, RecLastSeen), sizeof(((Voter *)0)->RecLastSeen), 1},
};
static const RecordField keyFields[] = {
    {offsetof(Voter, VotersIndexes_ID), sizeof(((Voter *)0)->VotersIndexes_ID), 0},
    {offsetof(Voter, DataHouse_ID), sizeof(((Voter *)0)->DataHouse_ID), 0},
    {offsetof(Voter, Gender), sizeof(((Voter *)0)->Gender), 1},
    {offsetof(Voter, UniqStateVoterID), sizeof(((Voter *)0)->UniqStateVoterID), 1},
    {offsetof(Voter, DataState_ID), sizeof(((Voter *)0)->DataState_ID), 0},
    {offsetof(Voter, RegParty), sizeof(((Voter *)0)->RegParty), 1},
    {offsetof(Voter, ReasonCode), sizeof(((Voter *)0)->ReasonCode), 1},
    {offsetof(Voter, Status), sizeof(((Voter *)0)->Status), 1},
    {offsetof(Voter, VotersMailingAddress_ID), sizeof(((Voter *)0)->VotersMailingAddress_ID), 0},
    {offsetof(Voter, IDRequired), sizeof(((Voter *)0)->IDRequired), 1},
    {offsetof(Voter, IDMet), sizeof(((Voter *)0)->IDMet), 1},
    {offsetof(Voter, ApplyDate), sizeof(((Voter *)0)->ApplyDate), 1},
    {offsetof(Voter, RegSource), sizeof(((Voter *)0)->RegSource), 1},
    {offsetof(Voter, DateInactive), sizeof(((Voter *)0)->DateInactive), 1},
    {offsetof(Voter, DatePurged), sizeof(((Voter *)0)->DatePurged), 1},
    {offsetof(Voter, CountyVoterNumber), sizeof(((Voter *)0)->CountyVoterNumber), 1},
    {offsetof(Voter, RMBActive), sizeof(((Voter *)0)->RMBActive), 1},
    {offsetof(Voter, RecFirstSeen), sizeof(((Voter *)0)->RecFirstSeen), 1},
    {offsetof(Voter, RecLastSeen), sizeof(((Voter *)0)->RecLastSeen), 1},
};

static int LoadVoterRows(MYSQL *db, Voter **records, size_t *count, size_t limit) {
    if (!records || !count) return EXIT_FAILURE;
    *records = NULL;
    *count = 0;
    if (!db) return EXIT_FAILURE;
    const char *select = "SELECT `Voters_ID`, `VotersIndexes_ID`, `DataHouse_ID`, `Voters_Gender`, `Voters_UniqStateVoterID`, `DataState_ID`, `Voters_RegParty`, `Voters_ReasonCode`, `Voters_Status`, `VotersMailingAddress_ID`, `Voters_IDRequired`, `Voters_IDMet`, `Voters_ApplyDate`, `Voters_RegSource`, `Voters_DateInactive`, `Voters_DatePurged`, `Voters_CountyVoterNumber`, `Voters_RMBActive`, `Voters_RecFirstSeen`, `Voters_RecLastSeen` FROM `Voters`"; char query[8192];
    int n = limit ? snprintf(query, sizeof query, "%s LIMIT %zu", select, limit) : snprintf(query, sizeof query, "%s", select);
    if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;
    void *data = NULL; size_t loaded = 0;
    int rc = LoadRecordArray(db, query, sizeof(Voter), offsetof(Voter, ID), offsetof(Voter, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);
    if (rc != EXIT_SUCCESS) { free(data); return rc; } *records = data; *count = loaded; return EXIT_SUCCESS;
}
int LoadVoterIntoMemory3(MYSQL *db, Voter **records, size_t *count) { return LoadVoterRows(db, records, count, 0); }
int LoadVoterIntoMemory4(MYSQL *db, Voter **records, size_t *count, size_t limit) { return LoadVoterRows(db, records, count, limit); }

void PrintVoter(const Voter *record) { if (!record) return; printf("ID=%" PRIu32, record->ID);
    printf(" VotersIndexes_ID=%" PRIu32, record->VotersIndexes_ID);
    printf(" DataHouse_ID=%" PRIu32, record->DataHouse_ID);
    printf(" Gender=%s", (record->nullFields & (UINT32_C(1) << 3)) ? "<NULL>" : record->Gender);
    printf(" UniqStateVoterID=%s", (record->nullFields & (UINT32_C(1) << 4)) ? "<NULL>" : record->UniqStateVoterID);
    printf(" DataState_ID=%" PRIu32, record->DataState_ID);
    printf(" RegParty=%s", (record->nullFields & (UINT32_C(1) << 6)) ? "<NULL>" : record->RegParty);
    printf(" ReasonCode=%s", (record->nullFields & (UINT32_C(1) << 7)) ? "<NULL>" : record->ReasonCode);
    printf(" Status=%s", (record->nullFields & (UINT32_C(1) << 8)) ? "<NULL>" : record->Status);
    printf(" VotersMailingAddress_ID=%" PRIu32, record->VotersMailingAddress_ID);
    printf(" IDRequired=%s", (record->nullFields & (UINT32_C(1) << 10)) ? "<NULL>" : record->IDRequired);
    printf(" IDMet=%s", (record->nullFields & (UINT32_C(1) << 11)) ? "<NULL>" : record->IDMet);
    printf(" ApplyDate=%s", (record->nullFields & (UINT32_C(1) << 12)) ? "<NULL>" : record->ApplyDate);
    printf(" RegSource=%s", (record->nullFields & (UINT32_C(1) << 13)) ? "<NULL>" : record->RegSource);
    printf(" DateInactive=%s", (record->nullFields & (UINT32_C(1) << 14)) ? "<NULL>" : record->DateInactive);
    printf(" DatePurged=%s", (record->nullFields & (UINT32_C(1) << 15)) ? "<NULL>" : record->DatePurged);
    printf(" CountyVoterNumber=%s", (record->nullFields & (UINT32_C(1) << 16)) ? "<NULL>" : record->CountyVoterNumber);
    printf(" RMBActive=%s", (record->nullFields & (UINT32_C(1) << 17)) ? "<NULL>" : record->RMBActive);
    printf(" RecFirstSeen=%s", (record->nullFields & (UINT32_C(1) << 18)) ? "<NULL>" : record->RecFirstSeen);
    printf(" RecLastSeen=%s", (record->nullFields & (UINT32_C(1) << 19)) ? "<NULL>" : record->RecLastSeen);
    putchar('\n'); }

int ParseVoterQuery(char *line, void *opaqueRecord) {
    Voter *record = opaqueRecord;
    if (!line || !record) return EXIT_FAILURE;
    memset(record, 0, sizeof *record);
    uint32_t nulls = 0;
    char *p = line;
    char *token0 = p; char *comma0 = strchr(p, ','); if (!comma0) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma0 = '\0'; p = comma0 + 1;
    if (*token0 == '\0') { nulls |= UINT32_C(1) << 1; } else {
        errno = 0; char *end0 = NULL; uintmax_t value0 = strtoumax(token0, &end0, 10); if (errno || end0 == token0 || *end0 || value0 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for VotersIndexes_ID: %s\n", token0); return EXIT_FAILURE; } record->VotersIndexes_ID = (uint32_t)value0;
    }
    char *token1 = p; char *comma1 = strchr(p, ','); if (!comma1) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma1 = '\0'; p = comma1 + 1;
    if (*token1 == '\0') { nulls |= UINT32_C(1) << 2; } else {
        errno = 0; char *end1 = NULL; uintmax_t value1 = strtoumax(token1, &end1, 10); if (errno || end1 == token1 || *end1 || value1 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataHouse_ID: %s\n", token1); return EXIT_FAILURE; } record->DataHouse_ID = (uint32_t)value1;
    }
    char *token2 = p; char *comma2 = strchr(p, ','); if (!comma2) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma2 = '\0'; p = comma2 + 1;
    if (*token2 == '\0') { nulls |= UINT32_C(1) << 3; } else {
        size_t len2 = strlen(token2); if (len2 >= sizeof record->Gender) { fprintf(stderr, "Gender exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Gender, token2, len2 + 1);
    }
    char *token3 = p; char *comma3 = strchr(p, ','); if (!comma3) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma3 = '\0'; p = comma3 + 1;
    if (*token3 == '\0') { nulls |= UINT32_C(1) << 4; } else {
        size_t len3 = strlen(token3); if (len3 >= sizeof record->UniqStateVoterID) { fprintf(stderr, "UniqStateVoterID exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->UniqStateVoterID, token3, len3 + 1);
    }
    char *token4 = p; char *comma4 = strchr(p, ','); if (!comma4) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma4 = '\0'; p = comma4 + 1;
    if (*token4 == '\0') { nulls |= UINT32_C(1) << 5; } else {
        errno = 0; char *end4 = NULL; uintmax_t value4 = strtoumax(token4, &end4, 10); if (errno || end4 == token4 || *end4 || value4 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for DataState_ID: %s\n", token4); return EXIT_FAILURE; } record->DataState_ID = (uint32_t)value4;
    }
    char *token5 = p; char *comma5 = strchr(p, ','); if (!comma5) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma5 = '\0'; p = comma5 + 1;
    if (*token5 == '\0') { nulls |= UINT32_C(1) << 6; } else {
        size_t len5 = strlen(token5); if (len5 >= sizeof record->RegParty) { fprintf(stderr, "RegParty exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->RegParty, token5, len5 + 1);
    }
    char *token6 = p; char *comma6 = strchr(p, ','); if (!comma6) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma6 = '\0'; p = comma6 + 1;
    if (*token6 == '\0') { nulls |= UINT32_C(1) << 7; } else {
        size_t len6 = strlen(token6); if (len6 >= sizeof record->ReasonCode) { fprintf(stderr, "ReasonCode exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ReasonCode, token6, len6 + 1);
    }
    char *token7 = p; char *comma7 = strchr(p, ','); if (!comma7) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma7 = '\0'; p = comma7 + 1;
    if (*token7 == '\0') { nulls |= UINT32_C(1) << 8; } else {
        size_t len7 = strlen(token7); if (len7 >= sizeof record->Status) { fprintf(stderr, "Status exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->Status, token7, len7 + 1);
    }
    char *token8 = p; char *comma8 = strchr(p, ','); if (!comma8) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma8 = '\0'; p = comma8 + 1;
    if (*token8 == '\0') { nulls |= UINT32_C(1) << 9; } else {
        errno = 0; char *end8 = NULL; uintmax_t value8 = strtoumax(token8, &end8, 10); if (errno || end8 == token8 || *end8 || value8 > UINT32_MAX) { fprintf(stderr, "Invalid unsigned integer for VotersMailingAddress_ID: %s\n", token8); return EXIT_FAILURE; } record->VotersMailingAddress_ID = (uint32_t)value8;
    }
    char *token9 = p; char *comma9 = strchr(p, ','); if (!comma9) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma9 = '\0'; p = comma9 + 1;
    if (*token9 == '\0') { nulls |= UINT32_C(1) << 10; } else {
        size_t len9 = strlen(token9); if (len9 >= sizeof record->IDRequired) { fprintf(stderr, "IDRequired exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->IDRequired, token9, len9 + 1);
    }
    char *token10 = p; char *comma10 = strchr(p, ','); if (!comma10) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma10 = '\0'; p = comma10 + 1;
    if (*token10 == '\0') { nulls |= UINT32_C(1) << 11; } else {
        size_t len10 = strlen(token10); if (len10 >= sizeof record->IDMet) { fprintf(stderr, "IDMet exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->IDMet, token10, len10 + 1);
    }
    char *token11 = p; char *comma11 = strchr(p, ','); if (!comma11) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma11 = '\0'; p = comma11 + 1;
    if (*token11 == '\0') { nulls |= UINT32_C(1) << 12; } else {
        size_t len11 = strlen(token11); if (len11 >= sizeof record->ApplyDate) { fprintf(stderr, "ApplyDate exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->ApplyDate, token11, len11 + 1);
    }
    char *token12 = p; char *comma12 = strchr(p, ','); if (!comma12) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma12 = '\0'; p = comma12 + 1;
    if (*token12 == '\0') { nulls |= UINT32_C(1) << 13; } else {
        size_t len12 = strlen(token12); if (len12 >= sizeof record->RegSource) { fprintf(stderr, "RegSource exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->RegSource, token12, len12 + 1);
    }
    char *token13 = p; char *comma13 = strchr(p, ','); if (!comma13) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma13 = '\0'; p = comma13 + 1;
    if (*token13 == '\0') { nulls |= UINT32_C(1) << 14; } else {
        size_t len13 = strlen(token13); if (len13 >= sizeof record->DateInactive) { fprintf(stderr, "DateInactive exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DateInactive, token13, len13 + 1);
    }
    char *token14 = p; char *comma14 = strchr(p, ','); if (!comma14) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma14 = '\0'; p = comma14 + 1;
    if (*token14 == '\0') { nulls |= UINT32_C(1) << 15; } else {
        size_t len14 = strlen(token14); if (len14 >= sizeof record->DatePurged) { fprintf(stderr, "DatePurged exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->DatePurged, token14, len14 + 1);
    }
    char *token15 = p; char *comma15 = strchr(p, ','); if (!comma15) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma15 = '\0'; p = comma15 + 1;
    if (*token15 == '\0') { nulls |= UINT32_C(1) << 16; } else {
        size_t len15 = strlen(token15); if (len15 >= sizeof record->CountyVoterNumber) { fprintf(stderr, "CountyVoterNumber exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->CountyVoterNumber, token15, len15 + 1);
    }
    char *token16 = p; char *comma16 = strchr(p, ','); if (!comma16) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma16 = '\0'; p = comma16 + 1;
    if (*token16 == '\0') { nulls |= UINT32_C(1) << 17; } else {
        size_t len16 = strlen(token16); if (len16 >= sizeof record->RMBActive) { fprintf(stderr, "RMBActive exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->RMBActive, token16, len16 + 1);
    }
    char *token17 = p; char *comma17 = strchr(p, ','); if (!comma17) { fprintf(stderr, "Too few comma-separated fields for Voter\n"); return EXIT_FAILURE; } *comma17 = '\0'; p = comma17 + 1;
    if (*token17 == '\0') { nulls |= UINT32_C(1) << 18; } else {
        size_t len17 = strlen(token17); if (len17 >= sizeof record->RecFirstSeen) { fprintf(stderr, "RecFirstSeen exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->RecFirstSeen, token17, len17 + 1);
    }
    char *token18 = p; if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Voter\n"); return EXIT_FAILURE; } p += strlen(p);
    if (*token18 == '\0') { nulls |= UINT32_C(1) << 19; } else {
        size_t len18 = strlen(token18); if (len18 >= sizeof record->RecLastSeen) { fprintf(stderr, "RecLastSeen exceeds its field capacity\n"); return EXIT_FAILURE; } memcpy(record->RecLastSeen, token18, len18 + 1);
    }
    if (strchr(p, ',')) { fprintf(stderr, "Too many comma-separated fields for Voter\n"); return EXIT_FAILURE; }
    memcpy(&record->nullFields, &nulls, sizeof nulls); return EXIT_SUCCESS;
}
int ParseVoterRecord(char *line, void *record) { return ParseVoterQuery(line, record); }

VoterHashTable *BuildVoterHashTable(const Voter *records, size_t count) {
    VoterHashTable *table = calloc(1, sizeof *table); if (!table) return NULL;
    table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof(Voter, ID), offsetof(Voter, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);
    if (!table->records) { free(table); return NULL; } return table;
}
VoterMatch FindVoterID(const VoterHashTable *table, const Voter *record) { VoterMatch m = {.status=RECORD_ERROR}; if (!table || !record) return m; return FindRecordID(table->records, record); }
void FreeVoterMatch(VoterMatch *match) { FreeRecordMatch(match); }
void FreeVoterHashTable(VoterHashTable *table) { if (table) { FreeRecordHashTable(table->records); free(table); } }

int QueryVoterMemory(const void *opaque, const void *record) {
    const VoterHashTable *table = opaque; const Voter *query = record; if (!table || !query) return EXIT_FAILURE; VoterMatch m = FindVoterID(table, query);
    if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) { printf("Found %zu matching ID(s):", m.count); for (size_t i=0; i<m.count; ++i) printf(" %" PRIu32, m.IDs[i]); putchar('\n'); } else if (m.status == RECORD_NOT_FOUND) puts("Address not found in memory."); else puts("In-memory lookup error."); int ok = m.status != RECORD_ERROR; FreeVoterMatch(&m); return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
    if (n > SIZE_MAX - *used - 1) return 0;
    size_t need = *used + n + 1;
    if (need > *cap) { size_t c = *cap ? *cap : 256; while (c < need) { if (c > SIZE_MAX/2) { c=need; break; } c*=2; } char *p=realloc(*buf,c); if(!p)return 0; *buf=p; *cap=c; }
    memcpy(*buf+*used,s,n); *used+=n; (*buf)[*used]='\0'; return 1;
}

int QueryVoterDatabase(MYSQL *db, const void *record) {
    const Voter *q=record; if(!db || !q) return EXIT_FAILURE; char *sql=NULL; size_t used=0,cap=0;
    const char *prefix = "SELECT `Voters_ID` FROM `Voters` WHERE "; if(!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;
    if (0 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 1)) { const char *part = "`VotersIndexes_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`VotersIndexes_ID` = %" PRIu32,q->VotersIndexes_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (1 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 2)) { const char *part = "`DataHouse_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataHouse_ID` = %" PRIu32,q->DataHouse_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (2 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 3)) { const char *part = "`Voters_Gender` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_Gender` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Gender); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Gender,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (3 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 4)) { const char *part = "`Voters_UniqStateVoterID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_UniqStateVoterID` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->UniqStateVoterID); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->UniqStateVoterID,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (4 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 5)) { const char *part = "`DataState_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`DataState_ID` = %" PRIu32,q->DataState_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (5 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 6)) { const char *part = "`Voters_RegParty` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_RegParty` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->RegParty); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->RegParty,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (6 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 7)) { const char *part = "`Voters_ReasonCode` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_ReasonCode` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ReasonCode); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ReasonCode,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (7 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 8)) { const char *part = "`Voters_Status` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_Status` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->Status); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->Status,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (8 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 9)) { const char *part = "`VotersMailingAddress_ID` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        char part[96]; int n=snprintf(part,sizeof part,"`VotersMailingAddress_ID` = %" PRIu32,q->VotersMailingAddress_ID); if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;
    }
    if (9 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 10)) { const char *part = "`Voters_IDRequired` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_IDRequired` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->IDRequired); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->IDRequired,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (10 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 11)) { const char *part = "`Voters_IDMet` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_IDMet` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->IDMet); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->IDMet,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (11 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 12)) { const char *part = "`Voters_ApplyDate` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_ApplyDate` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->ApplyDate); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->ApplyDate,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (12 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 13)) { const char *part = "`Voters_RegSource` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_RegSource` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->RegSource); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->RegSource,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (13 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 14)) { const char *part = "`Voters_DateInactive` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_DateInactive` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DateInactive); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DateInactive,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (14 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 15)) { const char *part = "`Voters_DatePurged` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_DatePurged` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->DatePurged); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->DatePurged,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (15 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 16)) { const char *part = "`Voters_CountyVoterNumber` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_CountyVoterNumber` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->CountyVoterNumber); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->CountyVoterNumber,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (16 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 17)) { const char *part = "`Voters_RMBActive` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_RMBActive` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->RMBActive); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->RMBActive,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (17 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 18)) { const char *part = "`Voters_RecFirstSeen` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_RecFirstSeen` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->RecFirstSeen); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->RecFirstSeen,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if (18 && !AppendSQL(&sql,&used,&cap," AND ",5)) goto sql_fail;
    if (q->nullFields & (UINT32_C(1) << 19)) { const char *part = "`Voters_RecLastSeen` IS NULL"; if(!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail; } else {
        const char *before = "`Voters_RecLastSeen` = '"; if(!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail; size_t len=strlen(q->RecLastSeen); if(len > (SIZE_MAX-1)/2) goto sql_fail; char *escaped=malloc(len*2+1); if(!escaped) goto sql_fail; unsigned long escaped_len=mysql_real_escape_string(db,escaped,q->RecLastSeen,(unsigned long)len); int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,"'",1); free(escaped); if(!append_ok) goto sql_fail;
    }
    if(mysql_query(db,sql)!=0) { fprintf(stderr, "Database lookup failed: %s\n",mysql_error(db)); free(sql); return EXIT_FAILURE; } free(sql); MYSQL_RES *res=mysql_store_result(db); if(!res) { fprintf(stderr, "Database result failed: %s\n",mysql_error(db)); return EXIT_FAILURE; } MYSQL_ROW row; size_t n=0; while((row=mysql_fetch_row(res))!=NULL) { if(row[0]) { printf("Database address ID: %s\n",row[0]); ++n; } } if(mysql_errno(db)) { fprintf(stderr, "Database result read failed: %s\n",mysql_error(db)); mysql_free_result(res); return EXIT_FAILURE; } if(!n) puts("Address not found in database."); mysql_free_result(res); return EXIT_SUCCESS;
sql_fail: free(sql); fprintf(stderr, "Could not build database lookup query.\n"); return EXIT_FAILURE;
}
