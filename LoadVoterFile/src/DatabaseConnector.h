#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H

#include "Voter.h"
#include <string>
#include <cppconn/connection.h>
#include <vector>
#include <iostream>
#include <exception>

// Database connection constants
const std::string  DB_HOST = "192.168.199.18";
const unsigned int DB_PORT = 3306;
const std::string  DB_USER = "usracct";
const std::string  DB_PASS = "usracct";
const std::string  DB_NAME = "RepMyBlockTwo";
	
#define DBFIELD_STATENAME    	"DataState"
#define DBFIELD_STATEABBREV  	"DataState"
#define DBFIELD_STREET       	"DataStreet"
#define DBFIELD_MIDDLENAME   	"DataMiddleName"
#define DBFIELD_LASTNAME     	"DataLastName"
#define DBFIELD_FIRSTNAME    	"DataFirstName"
#define DBFIELD_DISTRICTTOWN 	"DataDistrictTown"
#define DBFIELD_CITY         	"DataCity"
#define DBFIELD_NONSTDFORMAT 	"DataStreetNonStdFormat"
#define DBFIELD_COUNTY				"DataCounty"
#define DBFIELD_MAILADDRESS		"DataMailingAddress"
#define DBFIELD_VOTERS				"Voters"
#define DBFIELD_VOTERSIDX			"VoterIndexes"
#define DBFIELD_VOTERSCMINFO	"VotersComplementInfo"
#define DBFIELD_ADDRESS				"DataAddress"
#define DBFIELD_HOUSE					"DataHouse"
#define DBFIELD_DISTRICT			"DataDistrict"
#define DBFIELD_DSTRCTEMPO		"DataDistrictTemporal"
	
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
const unsigned int DBFIELDID_COUNTY				= 9;
const unsigned int DBFIELDID_MAILADDRESS	= 10;
const unsigned int DBFIELDID_VOTERS       = 11;
const unsigned int DBFIELDID_VOTERSIDX		= 12;
const unsigned int DBFIELDID_VOTERSCMINFO	= 13;
const unsigned int DBFIELDID_ADDRESS			= 14;
const unsigned int DBFIELDID_HOUSE				= 15;
const unsigned int DBFIELDID_DISTRICT			= 16;
const unsigned int DBFIELDID_DSTRCTEMPO		= 17;

class DatabaseConnector {
public:
	DatabaseConnector(const std::string StateAbbrev);
	~DatabaseConnector();
	bool connect();

	// These are the simple loads
	bool LoadFirstName(VoterMap& voterMap);
	bool LoadLastName(VoterMap& voterMap);
	bool LoadMiddleName(VoterMap& voterMap);
	bool LoadStateName(VoterMap& voterMap);
	bool LoadStateAbbrev(VoterMap& voterMap);
	bool LoadStreetName(VoterMap& voterMap);
	bool LoadDistrictTown(VoterMap& voterMap); 
	bool LoadCity(VoterMap& voterMap);
	bool LoadNonStdFormat(VoterMap& voterMap);
	bool LoadCounty(VoterMap& voterMap);
	
	// These are the simple loads
	bool TriggerSaveFirstNameDB(void);
	bool TriggerSaveLastNameDB(void);
	bool TriggerSaveMiddleNameDB(void);
	bool TriggerSaveStreetNameDB(void);
	bool TriggerSaveDistrictTownDB(void);
	bool TriggerSaveCityDB(void);
	bool TriggerSaveNonStdFormatDB(void);
	
	// These are the complex loads.
	bool LoadVoters(VoterMap& Map);
	bool LoadVotersIdx(VoterIdxMap& Map);
	bool LoadVotersComplementInfo(VoterComplementInfoMap& Map);
	bool LoadDataMailingAddress(DataMailingAddressMap& Map);
	bool LoadDataDistrict(DataDistrictMap& Map);
	bool LoadDataDistrictTemporal(DataDistrictTemporalMap& Map);
	bool LoadDataHouse(DataHouseMap& Map);
	bool LoadDataAddress(DataAddressMap& Map);
	
	// There are the complex saves.
	bool SaveDbVoters(VoterMap& Map);
	bool SaveDbVoterIdx(VoterIdxMap& Map);
	bool SaveDbVotersComplementInfo(VoterComplementInfoMap& Map);
	bool SaveDbDataMailingAddress(DataMailingAddressMap& Map);
	bool SaveDbDataDistrict(DataDistrictMap& Map);
	bool SaveDbDataDistrictTemporal(DataDistrictTemporalMap& Map);
	bool SaveDbDataHouse(DataHouseMap& Map);
	bool SaveDbDataAddress(DataAddressMap& Map);

	// These are to read the data of the simple loads.
	int ReturnIndex(const std::string& query);
	int CheckIndex(const std::string& query);
	std::string ListFieldNotFound(void);
	int PrintLatestID(int TableNameID);
	int ReturnStateID(void);
	
	// Return codes
	std::string genderToString(Gender gender);
  std::string reasonCodeToString(ReasonCode code);
	std::string statusToString(Status status);
  std::string regSourceToString(RegSource source);
	std::string boolToString(bool source);

	Gender stringToGender(const std::string& str);
	ReasonCode stringToReasonCode(const std::string& str);
	Status stringToStatus(const std::string& str);
	RegSource stringToRegSource(const std::string& str);
	bool stringToBool(const std::string& str);
      
private:
  sql::Connection* con;
 	std::map<std::string, int> dataMap;
 	int dbFieldType = -1;
 	std::vector<std::string> FieldToAddToDB;
 	int lastDBidFound = -1;
 	int StateID = 1;
 	
 	int SimpleLastDbID[TOTALDBFIELDS];
 	
	std::string RemoveAllSpacesString(const std::string& name);
	std::string CustomEscapeString(const std::string& input);

	std::string ToUpperAccents(const std::string& input);
	std::string ToLowerAccents(const std::string& input);
	std::string ReturnDBInjest(const std::string& query, int currentBatchCount);

	std::string nameCase(const std::string& input);
	std::string intToMySQLDate(int dateInt);
	int mysqlDateToInt(const std::string& mysqlDate);
  void executeSimpleQuery(const std::string& query, int DBCount);
	void executeSimpleSave(int DBCount);
	std::string returnInsertString(int DBCount);
   
  // Made these methods private as they are utility functions for the public query methods
  void executeLoadVotersQuery(const std::string& query, VoterMap& Map);
	void executeLoadVotersIdxQuery(const std::string& query, VoterIdxMap& Map);
	void executeLoadVotersComplementInfoQuery(const std::string& query, VoterComplementInfoMap& Map);
	void executeLoadDataMailingAddressQuery(const std::string& query, DataMailingAddressMap& Map);
	void executeLoadDataDistrictQuery(const std::string& query, DataDistrictMap& Map);
	void executeLoadDataDistrictTemporalQuery(const std::string& query, DataDistrictTemporalMap& Map);
	void executeLoadDataHouseQuery(const std::string& query, DataHouseMap& Map);
	void executeLoadDataAddressQuery(const std::string& query, DataAddressMap& Map);
		
		
	int LoadStateAbbrev(const std::string& StateAbbrev);

};

#endif //DATABASECONNECTOR_H