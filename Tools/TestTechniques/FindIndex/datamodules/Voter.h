typedef struct {
    uint32_t ID;
    uint32_t VotersIndexes_ID;
    uint32_t DataHouse_ID;
    char Gender[13];
    char UniqStateVoterID[201];
    uint32_t DataState_ID;
    char RegParty[13];
    char ReasonCode[20];
    char Status[26];
    uint32_t VotersMailingAddress_ID;
    char IDRequired[4];
    char IDMet[4];
    char ApplyDate[11];
    char RegSource[15];
    char DateInactive[11];
    char DatePurged[11];
    char CountyVoterNumber[201];
    char RMBActive[4];
    char RecFirstSeen[11];
    char RecLastSeen[11];
} Voter;


