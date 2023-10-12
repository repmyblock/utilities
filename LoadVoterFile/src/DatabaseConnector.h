#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H

#include "Voter.h"
#include <string>
#include <cppconn/connection.h>

// Database connection constants
const std::string  DB_HOST = "data.theochino.us";
const unsigned int DB_PORT = 3306;
const std::string  DB_USER = "usracct";
const std::string  DB_PASS = "usracct";
const std::string  DB_NAME = "RepMyBlockTwo";

const unsigned int DBFIELDID_STATENAME    = 0;
const unsigned int DBFIELDID_STATEABBREV  = 1;
const unsigned int DBFIELDID_STREET       = 2;
const unsigned int DBFIELDID_MIDDLENAME   = 3;
const unsigned int DBFIELDID_LASTNAME     = 4;
const unsigned int DBFIELDID_FIRSTNAME    = 5;
const unsigned int DBFIELDID_DISTRICTTOWN = 6;
const unsigned int DBFIELDID_CITY         = 7;
const unsigned int DBFIELDID_NONSTDFORMAT = 8;

const std::string  DBFIELD_STATENAME      = "DataState";
const std::string  DBFIELD_STATEABBREV    = "DataState";
const std::string  DBFIELD_STREET         = "DataStreet";
const std::string  DBFIELD_MIDDLENAME     = "DataMiddleName";
const std::string  DBFIELD_LASTNAME       = "DataLastName";
const std::string  DBFIELD_FIRSTNAME      = "DataFirstName";
const std::string  DBFIELD_DISTRICTTOWN   = "DataDistrictTown";
const std::string  DBFIELD_CITY           = "DataCity";
const std::string  DBFIELD_NONSTDFORMAT   = "DataStreetNonStdFormat";

class DatabaseConnector {
public:
	DatabaseConnector();
	~DatabaseConnector();
	bool connect(VoterMap& voterMap);
	bool connect(VoterIdxMap& voterIdxMap);

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

	// These are the complex loads.
	bool LoadVoters(VoterMap& Map);
	bool LoadVotersIdx(VoterIdxMap& Map);
	bool LoadVotersComplementInfo(VoterComplementInfoMap& Map);
	bool LoadDataMailingAddress(DataMailingAddressMap& Map);
	bool LoadDataDistrict(DataDistrictMap& Map);
	bool LoadDataDistrictTemporal(DataDistrictTemporalMap& Map);
	bool LoadDataHouse(DataHouseMap& Map);
	bool LoadDataAddress(DataAddressMap& Map);

	// These are to read the data of the simple loads.
	int ReturnIndex(const std::string& query);
    
private:
  sql::Connection* con;
 	std::map<std::string, int> dataMap;
 	unsigned int dbFieldType = -1;

	std::string ToUpperAccents(const std::string& input);
	std::string intToMySQLDate(int dateInt);
	int mysqlDateToInt(const std::string& mysqlDate);
  void executeSimpleQuery(const std::string& query, int DBCount);
 
  // Made these methods private as they are utility functions for the public query methods
  void executeLoadVotersQuery(const std::string& query, VoterMap& Map);
	void executeLoadVotersIdxQuery(const std::string& query, VoterIdxMap& Map);
	void executeLoadVotersComplementInfoQuery(const std::string& query, VoterComplementInfoMap& Map);
	void executeLoadDataMailingAddressQuery(const std::string& query, DataMailingAddressMap& Map);
	void executeLoadDataDistrictQuery(const std::string& query, DataDistrictMap& Map);
	void executeLoadDataDistrictTemporalQuery(const std::string& query, DataDistrictTemporalMap& Map);
	void executeLoadDataHouseQuery(const std::string& query, DataHouseMap& Map);
	void executeLoadDataAddressQuery(const std::string& query, DataAddressMap& Map);
};

#endif //DATABASECONNECTOR_H