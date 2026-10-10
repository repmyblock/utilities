typedef struct {
    uint32_t ID;
    char ZipCode[21];
    char DBTable[401];
    char DBTableValue[401];
    uint32_t DataCounty_ID;
    uint32_t DataDistrictTown_ID;
    uint16_t Electoral;
    uint16_t StateAssembly;
    uint8_t  StateSenate;
    uint16_t Legislative;
    char Ward[13];
    uint8_t Congress;
} DataDistrict;
