typedef struct {
    uint32_t ID;
    uint32_t Voters_ID;
    char PrevName[601];
    char PrevAddress[401];
    uint32_t DataCountyID_PrevCounty;
    uint16_t LastYearVoted;
    char LastDateVoted[11];
    char OtherParty[401];
} VoterComplementInfo;
