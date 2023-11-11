#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include "Voter.h"
#include "DatabaseConnector.h"
#include <string>
#include <vector>

#define DBFIELD_STATENAME     "DataState"
#define DBFIELD_STATEABBREV   "DataState"
#define DBFIELD_STREET        "DataStreet"
#define DBFIELD_MIDDLENAME    "DataMiddleName"
#define DBFIELD_LASTNAME      "DataLastName"
#define DBFIELD_FIRSTNAME     "DataFirstName"
#define DBFIELD_DISTRICTTOWN  "DataDistrictTown"
#define DBFIELD_CITY          "DataCity"
#define DBFIELD_NONSTDFORMAT  "DataStreetNonStdFormat"
#define DBFIELD_COUNTY        "DataCounty"
#define DBFIELD_MAILADDRESS   "DataMailingAddress"
#define DBFIELD_VOTERS        "Voters"
#define DBFIELD_VOTERSIDX     "VoterIndexes"
#define DBFIELD_VOTERSCMINFO  "VotersComplementInfo"
#define DBFIELD_ADDRESS       "DataAddress"
#define DBFIELD_HOUSE         "DataHouse"
#define DBFIELD_DISTRICT      "DataDistrict"
#define DBFIELD_DSTRCTEMPO    "DataDistrictTemporal"
  
#define TOTALDBFIELDS 17
const std::string FieldNames[] = { 
    DBFIELD_STATENAME, DBFIELD_STATEABBREV, DBFIELD_STREET, DBFIELD_MIDDLENAME, 
    DBFIELD_LASTNAME, DBFIELD_FIRSTNAME, DBFIELD_DISTRICTTOWN, DBFIELD_CITY, 
    DBFIELD_NONSTDFORMAT, DBFIELD_COUNTY, DBFIELD_MAILADDRESS, DBFIELD_VOTERS,
    DBFIELD_VOTERSIDX, DBFIELD_VOTERSCMINFO, DBFIELD_ADDRESS, DBFIELD_HOUSE,
    DBFIELD_DISTRICT, DBFIELD_DISTRICT, DBFIELD_DSTRCTEMPO
};

const unsigned int DBFIELDID_STATENAME    = 0;
const unsigned int DBFIELDID_STATEABBREV  = 1;
const unsigned int DBFIELDID_STREET       = 2;
const unsigned int DBFIELDID_MIDDLENAME   = 3;
const unsigned int DBFIELDID_LASTNAME     = 4;
const unsigned int DBFIELDID_FIRSTNAME    = 5;
const unsigned int DBFIELDID_DISTRICTTOWN = 6;
const unsigned int DBFIELDID_CITY         = 7;
const unsigned int DBFIELDID_NONSTDFORMAT = 8;
const unsigned int DBFIELDID_COUNTY       = 9;
const unsigned int DBFIELDID_MAILADDRESS  = 10;
const unsigned int DBFIELDID_VOTERS       = 11;
const unsigned int DBFIELDID_VOTERSIDX    = 12;
const unsigned int DBFIELDID_VOTERSCMINFO = 13;
const unsigned int DBFIELDID_ADDRESS      = 14;
const unsigned int DBFIELDID_HOUSE        = 15;
const unsigned int DBFIELDID_DISTRICT     = 16;
const unsigned int DBFIELDID_DSTRCTEMPO   = 17;

class DataCollector {
public:
  DataCollector(DatabaseConnector& conn) : dbConnection(conn) {};
  // DataCollector() = default;
  void collectData();
  void PrintVoterTable(const VoterMap&);
  void executeQuery(const std::string&);
  int returnNumberOfEntries(void);
  int returnQueryTimes(void);

  // These are the simple loads
  bool LoadFirstName(VoterMap&); // Fixed
  bool LoadLastName(VoterMap&);
  bool LoadMiddleName(VoterMap&);
  bool LoadStateName(VoterMap&);
  bool LoadStateAbbrev(VoterMap&);
  bool LoadStreetName(VoterMap&);
  bool LoadDistrictTown(VoterMap&); 
  bool LoadCity(VoterMap&);
  bool LoadNonStdFormat(VoterMap&);
  bool LoadCounty(VoterMap&);
  
  // These are the simple loads
  bool TriggerSaveFirstNameDB(void);
  bool TriggerSaveLastNameDB(void);
  bool TriggerSaveMiddleNameDB(void);
  bool TriggerSaveStreetNameDB(void);
  bool TriggerSaveDistrictTownDB(void);
  bool TriggerSaveCityDB(void);
  bool TriggerSaveNonStdFormatDB(void);
  
  // These are the complex loads.
  bool LoadData(DataMailingAddressMap&);
  bool LoadData(VoterMap&);
  bool LoadData(VoterIdxMap&);
  bool LoadData(VoterComplementInfoMap&);
  bool LoadData(DataDistrictMap&);
  bool LoadData(DataDistrictTemporalMap&);
  bool LoadData(DataHouseMap&);
  bool LoadData(DataAddressMap&);
  
  // There are the complex saves.
  bool SaveDataBase(VoterMap&);
  bool SaveDataBase(VoterIdxMap&);
  bool SaveDataBase(VoterComplementInfoMap&);
  bool SaveDataBase(DataMailingAddressMap&);
  bool SaveDataBase(DataDistrictMap&);
  bool SaveDataBase(DataDistrictTemporalMap&);
  bool SaveDataBase(DataHouseMap&);
  bool SaveDataBase(DataAddressMap&);
  
  // To Print the data
  void PrintTable(const VoterMap&);
  void PrintTable(VoterComplementInfoMap&);
  void PrintTable(DataMailingAddressMap&);

  // These are to read the data of the simple loads.
  int ReturnIndex(const std::string&);
  int CheckIndex(const std::string&);
  std::string ListFieldNotFound(void);
  int PrintLatestID(int);
  int ReturnStateID(void);
  
  // Return codes
  std::string genderToString(Gender);
  std::string reasonCodeToString(ReasonCode);
  std::string statusToString(Status);
  std::string regSourceToString(RegSource);
  std::string boolToString(bool);

  Gender stringToGender(const std::string&);
  ReasonCode stringToReasonCode(const std::string&);
  Status stringToStatus(const std::string&);
  RegSource stringToRegSource(const std::string&);
  bool stringToBool(const std::string&);
    
  uint32_t simpleHash(const std::string&);
    
  int countFoundinDB (void);
  int countNotFoundinDB (void);
      
private:
  DatabaseConnector& dbConnection;
  sql::Connection* con;
  std::chrono::milliseconds duration;

  int dbFieldType = -1;
  std::vector<std::string> FieldToAddToDB;
  std::map<std::string, int> dataMap;
  int lastDBidFound = -1;
  int StateID = 1;
  int CountFoundinDB = 0;
  int CountNotFoundinDB = 0;
  
  int SimpleLastDbID[TOTALDBFIELDS];
  
  std::string RemoveAllSpacesString(const std::string&);
  std::string CustomEscapeString(const std::string&);

  std::string ToUpperAccents(const std::string&);
  std::string ToLowerAccents(const std::string&);
  std::string ReturnDBInjest(const std::string&, int);

  std::string nameCase(const std::string&);
  std::string intToMySQLDate(int);
  int mysqlDateToInt(const std::string&);

  void executeSimpleQuery(const std::string&, int);                       // Fixed    
    
  void executeSimpleSave(int);
  std::string returnInsertString(int);
   
  // Made these methods private as they are utility functions for the public query methods
  void executeLoadDataQuery(const std::string&, DataMailingAddressMap&);
  void executeLoadDataQuery(const std::string&, VoterMap&);
  void executeLoadDataQuery(const std::string&, VoterIdxMap&);
  void executeLoadDataQuery(const std::string&, VoterComplementInfoMap&);
  void executeLoadDataQuery(const std::string&, DataDistrictMap&);
  void executeLoadDataQuery(const std::string&, DataDistrictTemporalMap&);
  void executeLoadDataQuery(const std::string&, DataHouseMap&);
  void executeLoadDataQuery(const std::string&, DataAddressMap&);
    
    
  int LoadStateAbbrev(const std::string&);
  inline uint32_t leftRotate(uint32_t, uint32_t);
  std::string uintToString(uint32_t);
  
  void exitIfSequenceFound(const std::string&, const std::string);
  void PrintLineAsHex(const std::string&);

};

#endif //DATACOLLECTOR_H