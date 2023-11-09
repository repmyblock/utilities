
#include "Voter.h"
#include "DataCollector.h"
#include "DatabaseConnector.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm> 
#include <cctype>
#include <optional>
#include <string>
#include <cstdint>

#define COUNTER         100000
#define SQLBATCH        10

#define SQL_QUERY_START	 	sql::ResultSet* res = dbConnection.executeQuery(sql);  
#define SQL_QUERY_END     dbConnection.deleteResource(res);
#define SQL_INSERT				dbConnection.executeInsert(sql);
	
#define CLOCK_START     auto start = std::chrono::high_resolution_clock::now();
#define CLOCK_END				auto end = std::chrono::high_resolution_clock::now();  \
                        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                          
#define PRINT_COUNTER   if ( ++Counter % COUNTER == 0 ) { std::cout << "Voter VotersComplementInfo: " << Counter << std::endl;  }
  
#define CHECK_FIELD     if (dbFieldType > -1) { std::cerr << "DB Field Type is set for " << FieldNames[dbFieldType] \
                                                          << " and can't be used anymore" << std::endl; exit(1); }
                                                            
#define SQL_EXEPTION    catch (sql::SQLException &e) {  std::cerr << "SQL Query: " << sql << std::endl; \
																												std::cerr << "Error: Could not execute query: " << PINK << e.what() << NC << std::endl; \
                                                        std::cerr << "Error code: " << e.getErrorCode() << std::endl; \
                                                        exit(1); }

#define SQL_INT_OR_NIL(str) (res->isNull(str)? NIL : res->getInt(str))
#define SQL_UPPERSTR_OR_NIL(str) (res->isNull(str)) ? NILSTRG : ToUpperAccents(res->getString(str))



#include <iostream>

/*
void DataCollector::DataCollector() : con(nullptr) {}

void DataCollector::~DataCollector() {
	if(con != nullptr) {
	  delete con;
	}
}
*/

void DataCollector::collectData() {
    std::cout << "Collecting data..." << std::endl;   	
}

int DataCollector::returnNumberOfEntries(void) {
		return dataMap.size();
}

int DataCollector::returnQueryTimes(void) {
	return duration.count();
}

/*
DataCollector::DataCollector(const std::string StateAbbrev) : con(nullptr) {
  if (connect()) { 
    StateID = LoadStateAbbrev(StateAbbrev);
  } else {
    std::cerr << "Failed to connect to the database." << std::endl;
    exit(1);
  }
}
*/






/*
To Save Space and remove the redundant but the incoming variable's name MUST be sql.
void DatabaseConnector::XXXNAME_FUNCTION_XXXXX(const std::string& sql, DataHouseMap& Map) {
  // Replaced with CLOCK_START
  auto start = std::chrono::high_resolution_clock::now();

  try {   
    // Replaced with SQL_QUERY_START  
    sql::Statement* stmt = con->createStatement(); 
    sql::ResultSet* res = stmt->executeQuery(sql);
 
    //
    // The SQL ITSELF MUST BE HERE
    //

    // Replaced with SQL_QUERY_END
    delete res; 
    delete stmt;
  } 
  // Replaced with SQL_EXEPTION
  catch (sql::SQLException &e) { std::cerr << "Error: Could not execute query. " << e.what() << std::endl; exit(1); }
  
  // Replaced with CLOCK_END
  auto end = std::chrono::high_resolution_clock::now(); 
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Execution time in " << duration.count() << " milliseconds." << std::endl;
*/

int DataCollector::CheckIndex(const std::string& query) {
  if (query.length() < 1) return NIL;
  	
  if (dataMap[query] == 0 ) { 
    FieldToAddToDB.push_back(query);
    dataMap[query] = -1; 
		CountNotFoundinDB++;
  } else {
  	CountFoundinDB++;
  }
     
  return dataMap[query];
}

int DataCollector::ReturnIndex(const std::string& query) {
  if (query.length() < 1) return NIL;
  if (dataMap[query] == 0 ) { dataMap[query] = -1; }
  return dataMap[query];
}

int DataCollector::PrintLatestID(int TableNameID) {
  return SimpleLastDbID[TableNameID];
}

int DataCollector::ReturnStateID(void) {
  return StateID;
}

std::string DataCollector::ListFieldNotFound(void) {
  std::string result;
  for(const std::string& str : FieldToAddToDB) {
      result += str + ' ';
  }
  return result;
}

std::string DataCollector::ReturnDBInjest(const std::string& input, int currentBatchCount) {
  std::string result;
    
  if(currentBatchCount > 0) { return ',' + input; }
  return input;
}

int DataCollector::countFoundinDB (void) {
	return CountFoundinDB;
}

int DataCollector::countNotFoundinDB (void) {
	return CountNotFoundinDB;
}

void DataCollector::executeSimpleSave(int DBCount) {
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;

  CLOCK_START
  try {
    for(const std::string& str : FieldToAddToDB) {
      prev_size = sql.length(); 

			if (str.length() > 0) {
	      switch (DBCount) {    
  	      case DBFIELDID_STREET: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\")", currentBatchCount); break;
    	    case DBFIELDID_MIDDLENAME: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\",\"" +  dbConnection.CustomEscapeString(RemoveAllSpacesString(str)) + "\")", currentBatchCount); break;
	        case DBFIELDID_LASTNAME: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\",\"" + dbConnection.CustomEscapeString(RemoveAllSpacesString(str)) + "\")", currentBatchCount); break;
	        case DBFIELDID_FIRSTNAME: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\",\"" + dbConnection.CustomEscapeString(RemoveAllSpacesString(str)) + "\")", currentBatchCount); break;
	        case DBFIELDID_DISTRICTTOWN: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\")", currentBatchCount); break;
	        case DBFIELDID_CITY: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\")", currentBatchCount); break;
	        case DBFIELDID_NONSTDFORMAT: sql += ReturnDBInjest("(null, \"" + dbConnection.CustomEscapeString(nameCase(str)) + "\")", currentBatchCount); break;
	      }
	      
	      if (sql.length() > prev_size) { currentBatchCount++;  SaveLast = true; }
	      if(currentBatchCount == batchSize || &str == &FieldToAddToDB.back()) {
	        if( currentBatchCount == batchSize) {
	        	sql = returnInsertString(DBCount) + sql;
	          SQL_INSERT
	          sql.clear();
	          currentBatchCount = 0;
	          SaveLast = false;
	        }
	      }	       
	    }
	  }
	      
    if( SaveLast == true && sql.length() > 0 ) {
    	sql = returnInsertString(DBCount) + sql;
      SQL_INSERT
      sql.clear();
    }             
   } SQL_EXEPTION  
     
  CLOCK_END
}

std::string DataCollector::returnInsertString(int DBCount) {
  switch (DBCount) {
    case DBFIELDID_STREET:        return "INSERT INTO " + std::string(DBFIELD_STREET) + " VALUES "; break;
    case DBFIELDID_MIDDLENAME:    return "INSERT INTO " + std::string(DBFIELD_MIDDLENAME) + " VALUES "; break;
    case DBFIELDID_LASTNAME:      return "INSERT INTO " + std::string(DBFIELD_LASTNAME) + " VALUES "; break;
    case DBFIELDID_FIRSTNAME:     return "INSERT INTO " + std::string(DBFIELD_FIRSTNAME) + " VALUES ";  break;
    case DBFIELDID_DISTRICTTOWN:  return "INSERT INTO " + std::string(DBFIELD_DISTRICTTOWN) + " VALUES "; break;
    case DBFIELDID_CITY:          return "INSERT INTO " + std::string(DBFIELD_CITY) + " VALUES "; break;
    case DBFIELDID_NONSTDFORMAT:  return "INSERT INTO " + std::string(DBFIELD_NONSTDFORMAT) + " VALUES "; break;
  }
  std::cout << RED << "Problem constructing SQL string in returnInsertString in DBCount: " << FieldNames[DBCount] << NC << std::endl;
  exit(1);
}

void DataCollector::executeSimpleQuery(const std::string& sql, int DBCount) {
  CLOCK_START
  
  try {
    sql::ResultSet* res = dbConnection.executeQuery(sql);   
    std::string MaxQuery;
    std::string idName, textName;
    switch (DBCount) {
      case DBFIELDID_STATEABBREV:
        idName = FieldNames[DBCount] + "_ID"; textName = FieldNames[DBCount] + "_Abbrev";
        break;
      case DBFIELDID_COUNTY:        
        idName = FieldNames[DBCount] + "_ID"; textName = FieldNames[DBCount] + "_BOEID";
        break;
      case DBFIELDID_STATENAME:     
      case DBFIELDID_DISTRICTTOWN:  
      case DBFIELDID_CITY:          
      case DBFIELDID_STREET:        
        idName = FieldNames[DBCount] + "_ID"; textName = FieldNames[DBCount] + "_Name";
        break;
      case DBFIELDID_MIDDLENAME:    
      case DBFIELDID_LASTNAME:      
      case DBFIELDID_FIRSTNAME:     
      case DBFIELDID_NONSTDFORMAT:  
        idName = FieldNames[DBCount] + "_ID"; textName = FieldNames[DBCount] + "_Text";
        break;
    }

    while (res->next()) {
      int index = res->getInt(idName);
      std::string name = ToUpperAccents(res->getString(textName));
      dataMap[name] = index;
      if ( index > SimpleLastDbID[DBCount]) { SimpleLastDbID[DBCount] = index; }
    }

    dbConnection.deleteResource(res);

  } SQL_EXEPTION
  CLOCK_END
}

/*******************
 *** VOTER TABLE ***
 *******************
  Table: Voters
    VotersIndexes_ID int UN 
    DataHouse_ID int UN 
    Voters_Gender enum('male','female','other','undetermined') 
    Voters_UniqStateVoterID varchar(50) 
    DataState_ID int UN 
    Voters_RegParty char(3) 
    Voters_ReasonCode enum('AdjudgedIncompetent','Death','Duplicate','Felon','MailCheck','MovedOutCounty','NCOA','NVRA','ReturnMail','VoterRequest','Other','Court','Inactive') 
    Voters_Status enum('Active','ActiveMilitary','ActiveSpecialFederal','ActiveSpecialPresidential','ActiveUOCAVA','Inactive','Purged','Prereg17YearOlds','Confirmation') 
    VotersMailingAddress_ID int UN 
    Voters_IDRequired enum('yes','no') 
    Voters_IDMet enum('yes','no') 
    Voters_ApplyDate date 
    Voters_RegSource enum('Agency','CBOE','DMV','LocalRegistrar','MailIn','School','OVR') 
    Voters_DateInactive date 
    Voters_DatePurged date 
    Voters_CountyVoterNumber varchar(50) 
    Voters_RMBActive enum('yes','no') 
    Voters_RecFirstSeen date 
    Voters_RecLastSeen date
*/


bool DataCollector::LoadData(VoterMap& Map) {
  executeLoadDataQuery("SELECT * FROM Voters WHERE DataState_ID = \"" + std::to_string(StateID) + "\"", Map);
  return true;
}

void DataCollector::executeLoadDataQuery(const std::string& sql, VoterMap& Map) {
  CLOCK_START 
  try {     
    SQL_QUERY_START

    while (res->next()) {
      int index = res->getInt("Voters_ID"); 

      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in LoadVoters ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }
      
      Map[Voter(
        SQL_INT_OR_NIL("VotersIndexes_ID"), SQL_INT_OR_NIL("DataHouse_ID"), stringToGender(ToUpperAccents(res->getString("Voters_Gender"))),
        SQL_UPPERSTR_OR_NIL("Voters_UniqStateVoterID"), SQL_UPPERSTR_OR_NIL("Voters_RegParty"), 
        stringToReasonCode(res->getString("Voters_ReasonCode")), stringToStatus(res->getString("Voters_Status")), 
        SQL_INT_OR_NIL("VotersMailingAddress_ID"), stringToBool(res->getString("Voters_IDRequired")), stringToBool(res->getString("Voters_IDMet")), 
        mysqlDateToInt(res->getString("Voters_ApplyDate")), stringToRegSource(res->getString("Voters_RegSource")), 
        mysqlDateToInt(res->getString("Voters_DateInactive")), mysqlDateToInt(res->getString("Voters_DatePurged")), 
        SQL_UPPERSTR_OR_NIL("Voters_CountyVoterNumber"), stringToBool(res->getString("Voters_RMBActive"))
      )] = index;

      if ( index > SimpleLastDbID[DBFIELDID_VOTERS]) { SimpleLastDbID[DBFIELDID_VOTERS] = index; }
      // PRINT_COUNTER
    }

    SQL_QUERY_END
  } SQL_EXEPTION  
  CLOCK_END
}

bool DataCollector::SaveDataBase(VoterMap& Map) { 
	
	std::cout << HI_WHITE << "Saving Voters" << NC << std::endl;
		
	const std::string query = "INSERT INTO Voters VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;
  CLOCK_START
    
  if (StateID < 1) {
    std::cout << HI_RED << "State ID not defined SaveDbVoters" << NC << std::endl;
    exit(1);
  }
        
  /*
    struct Voter {
      int votersIndexesId; int dataHouseId; Gender gender; std::string uniqStateVoterId; std::string regParty;
      ReasonCode reasonCode; Status status; int mailingAddressId; bool idRequired; bool idMet; int applyDate;
      RegSource regSource; int dateInactive; int datePurged; std::string countyVoterNumber; bool rmbActive;
  */
  
  try {      
  	auto it = Map.begin();
    while (it != Map.end()) {
	 		const Voter& voter = it->first;
 	    
      if (Map[voter] == 0) {
      	if (voter.uniqStateVoterId.length() > 0) {
	        std::string tmpsql = "null,";   
	        tmpsql += (voter.votersIndexesId > 0) ? ("\"" + std::to_string(voter.votersIndexesId) + "\",") : "null,"; 
	        tmpsql += (voter.dataHouseId > 0) ? ("\"" + std::to_string(voter.dataHouseId) + "\",") : "null,"; 
	        tmpsql += (voter.gender != Gender::Undefined) ? ("\"" + genderToString(voter.gender) + "\",") : "null,";
	        tmpsql += (voter.uniqStateVoterId.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(voter.uniqStateVoterId)) + "\",") : "null,";
	        tmpsql +=  "\"" + std::to_string(StateID) + "\",";
	        tmpsql += (voter.regParty.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(voter.regParty)) + "\",") : "null,";
	        tmpsql += (voter.reasonCode != ReasonCode::Undefined) ? ("\"" + reasonCodeToString(voter.reasonCode) + "\",") : "null,";    
	        tmpsql += (voter.status != Status::Undefined) ? ("\"" + statusToString(voter.status) + "\",") : "null,";    
	        tmpsql += (voter.mailingAddressId > 0) ? ("\"" + std::to_string(voter.mailingAddressId) + "\",") : "null,";   
	        tmpsql += "\"" + boolToString(voter.idRequired) + "\","; 
	        tmpsql += "\"" + boolToString(voter.idMet) + "\",";
	        tmpsql += (voter.applyDate > 0) ? ("\"" + std::to_string(voter.applyDate) + "\",") : "null,";   
	        tmpsql += "\"" + regSourceToString(voter.regSource) + "\","; 
	        tmpsql += (voter.dateInactive > 0) ? ("\"" + std::to_string(voter.dateInactive) + "\",") : "null,";   
	        tmpsql += (voter.datePurged > 0) ? ("\"" + std::to_string(voter.datePurged) + "\",") : "null,";   
	        tmpsql += (voter.countyVoterNumber.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(voter.countyVoterNumber)) + "\",") : "null,";    
	        tmpsql += (voter.rmbActive > 0) ? ("\"" + std::to_string(voter.rmbActive) + "\",") : "null,";   
	        tmpsql += "NOW(), NOW()";
                  
	        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
	        ++currentBatchCount;
	        SaveLast = true;
	      } 
	      it = Map.erase(it);
      } else {
      	++it;
      }
      
      if( currentBatchCount == batchSize) {
        sql = "INSERT INTO Voters VALUES " + sql;
        SQL_INSERT
        sql.clear();  
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true ) {
      sql = "INSERT INTO Voters VALUES " + sql;
      SQL_INSERT
      sql.clear();
    }
    
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM Voters";
  if ( SimpleLastDbID[DBFIELDID_VOTERS] > 0) {
    sql += " WHERE Voters_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_VOTERS]);   
  }
    
  executeLoadDataQuery(sql, Map);
  return true;
}


void DataCollector::PrintTable(const VoterMap& Map) {
	int Counter = 0;
	for(auto it = Map.begin(); it != Map.end(); ++it) {
	  const Voter& voter = it->first;
	  int TableId = it->second;

  	std::cout << HI_WHITE << ++Counter << NC << " - Uniq ID: " << HI_PINK << voter.uniqStateVoterId << NC << std::endl;	  
	  std::cout << "\tIndexesID:\t"     << HI_YELLOW << voter.votersIndexesId << NC << std::endl;
  	std::cout << "\tHouseID:\t"       << HI_YELLOW << voter.dataHouseId << NC << std::endl;
  	std::cout << "\tGender:\t\t"      << HI_YELLOW << genderToString(voter.gender) << NC << std::endl;
  	std::cout << "\tParty Code:\t"    << HI_YELLOW << voter.regParty << NC << std::endl;
  	std::cout << "\tReason Code:\t"   << HI_YELLOW << reasonCodeToString(voter.reasonCode) << NC << std::endl;
  	std::cout << "\tStatus:\t\t"      << HI_YELLOW << statusToString(voter.status) << NC << std::endl;
  	std::cout << "\tMailing AddID:\t" << HI_YELLOW << voter.mailingAddressId << NC << std::endl;
  	std::cout << "\tID Required:\t"   << HI_YELLOW << boolToString(voter.idRequired) << NC << std::endl;
  	std::cout << "\tID Met:\t\t"      << HI_YELLOW << boolToString(voter.idMet) << NC << std::endl;
  	std::cout << "\tApply Date:\t"    << HI_YELLOW << voter.applyDate << NC << std::endl;
  	std::cout << "\tReg Source:\t"    << HI_YELLOW << regSourceToString(voter.regSource) << NC << std::endl;
  	std::cout << "\tDate Inactive:\t" << HI_YELLOW << voter.dateInactive << NC << std::endl;
  	std::cout << "\tDate Purged:\t"   << HI_YELLOW << voter.datePurged << NC << std::endl;
  	std::cout << "\tCounty Reg #:\t"  << HI_YELLOW << voter.countyVoterNumber << NC << std::endl;
  	std::cout << "\tRMB Active:\t"    << HI_YELLOW << boolToString(voter.rmbActive) << NC << std::endl;
		std::cout << std::endl;
	}
}


/**************************
 *** VOTERINDEXES TABLE ***
 **************************
  Table: VotersIndexes
    VotersIndexes_ID int UN AI PK 
    DataLastName_ID int UN 
    DataFirstName_ID int UN 
    DataMiddleName_ID int UN 
    VotersIndexes_Suffix varchar(10) 
    VotersIndexes_DOB date 
    DataState_ID int UN 
    VotersIndexes_UniqStateVoterID char(20)
*/


void DataCollector::executeLoadDataQuery(const std::string& sql, VoterIdxMap& Map) {
  CLOCK_START
  try {
    SQL_QUERY_START
    while (res->next()) {
      int index = res->getInt("VotersIndexes_ID");

      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in LoadVotersIdx ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }

      Map[VoterIdx(
        SQL_INT_OR_NIL("DataLastName_ID"), SQL_INT_OR_NIL("DataFirstName_ID"),
        SQL_INT_OR_NIL("DataMiddleName_ID"), SQL_UPPERSTR_OR_NIL("VotersIndexes_Suffix"),
        res->isNull("VotersIndexes_DOB") ? NIL : mysqlDateToInt(res->getString("VotersIndexes_DOB")), 
        SQL_UPPERSTR_OR_NIL("VotersIndexes_UniqStateVoterID")
      )] = index;

      if ( index > SimpleLastDbID[DBFIELDID_VOTERSIDX]) { SimpleLastDbID[DBFIELDID_VOTERSIDX] = index; }
      // PRINT_COUNTER
    }

    SQL_QUERY_END
  } SQL_EXEPTION  
  CLOCK_END
}

bool DataCollector::LoadData(VoterIdxMap& Map) {
  CHECK_FIELD
  executeLoadDataQuery("SELECT * FROM VotersIndexes WHERE DataState_ID = \"" + std::to_string(StateID) + "\"", Map);
  return true;
}

bool DataCollector::SaveDataBase(VoterIdxMap& Map) {  
	
	std::cout << HI_WHITE << "Saving VotersIndexes" << NC << std::endl;
		
	const std::string query = "INSERT INTO VotersIndexes VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;
  CLOCK_START

  try {
    auto it = Map.begin();
    while (it != Map.end()) {
      const VoterIdx& voterIdx = it->first;

      if (Map[voterIdx] == 0 && voterIdx.dataUniqStateId.length() > 0) {
        std::string tmpsql = "null,";
        tmpsql += (voterIdx.dataLastNameId > 0) ? ("\"" + std::to_string(voterIdx.dataLastNameId) + "\",") : "null,";     
        tmpsql += (voterIdx.dataFirstNameId > 0) ? ("\"" + std::to_string(voterIdx.dataFirstNameId) + "\",") : "null,";     
        tmpsql += (voterIdx.dataMiddleNameId > 0) ? ("\"" + std::to_string(voterIdx.dataMiddleNameId) + "\",") : "null,";     
        tmpsql += (voterIdx.dataNameSuffix.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(voterIdx.dataNameSuffix)) + "\",") : "null,";     
        tmpsql += (voterIdx.dataDOB > 0) ? ("\"" + std::to_string(voterIdx.dataDOB) + "\",") : "null,";     
        tmpsql += (StateID > 0) ? ("\"" + std::to_string(StateID) + "\",") : "null,";     
        tmpsql += (voterIdx.dataUniqStateId.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(voterIdx.dataUniqStateId)) + "\"") : "null";     
       
        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
        ++currentBatchCount;
        SaveLast = true;
        it = Map.erase(it);
      } else {
      	++it;
      }
      
      if( currentBatchCount == batchSize) {
        sql = "INSERT INTO VotersIndexes VALUES " + sql;
        SQL_INSERT
        sql.clear();  
        currentBatchCount = 0;
        SaveLast = false;
      }
    }
  
    if( SaveLast == true ) {
      sql = "INSERT INTO VotersIndexes VALUES " + sql;
  		SQL_INSERT
      sql.clear();
    }
    
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM VotersIndexes";
  if ( SimpleLastDbID[DBFIELDID_VOTERSIDX] > 0) {
    sql += " WHERE VotersIndexes_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_VOTERSIDX]);   
  }
 
  executeLoadDataQuery(sql, Map);
  Map[VoterIdx(NIL,NIL,NIL,NILSTRG,NIL,NILSTRG)] = -2;
  return true;
}

/*********************************
 *** VOTERCOMPLEMENTINFO TABLE ***
 *********************************
  Table: VotersComplementInfo
    VotersComplementInfo_ID int UN AI PK 
    Voters_ID int UN 
    VotersComplementInfo_PrevName varchar(150) 
    VotersComplementInfo_PrevAddress varchar(100) 
    DataCountyID_PrevCounty int UN 
    VotersComplementInfo_LastYearVoted year 
    VotersComplementInfo_LastDateVoted date 
    VotersComplementInfo_OtherParty varchar(100)
*/

void DataCollector::executeLoadDataQuery(const std::string& sql, VoterComplementInfoMap& Map){
  CLOCK_START 
  try {     
    SQL_QUERY_START

    while (res->next()) {
      
      int index = res->getInt("VotersComplementInfo_ID");
      
      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in Voter Complement Info ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }     
      
      Map[VoterComplementInfo(
        SQL_INT_OR_NIL("Voters_ID"), SQL_UPPERSTR_OR_NIL("VotersComplementInfo_PrevName"),
        SQL_UPPERSTR_OR_NIL("VotersComplementInfo_PrevAddress"), SQL_INT_OR_NIL("DataCountyID_PrevCounty"),
        SQL_INT_OR_NIL("VotersComplementInfo_LastYearVoted"), SQL_INT_OR_NIL("VotersComplementInfo_LastDateVoted"),
        SQL_UPPERSTR_OR_NIL("VotersComplementInfo_OtherParty")
      )] = index;
      
      if ( index > SimpleLastDbID[DBFIELDID_VOTERSCMINFO]) { SimpleLastDbID[DBFIELDID_VOTERSCMINFO] = index; }
    }

    SQL_QUERY_END
  } SQL_EXEPTION  
  CLOCK_END
}

bool DataCollector::LoadData(VoterComplementInfoMap& Map) {
  CHECK_FIELD
  executeLoadDataQuery("SELECT * FROM VotersComplementInfo", Map);
  return true;
}

bool DataCollector::SaveDataBase(VoterComplementInfoMap& Map) { 
	
	std::cout << HI_WHITE << "Saving VotersComplementInfo" << NC << std::endl;
		
	const std::string query = "INSERT INTO VotersComplementInfo VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;
  CLOCK_START

  try {
		auto it = Map.begin();
    while (it != Map.end()) {
      const VoterComplementInfo& voterComplementInfo = it->first;
          
      if (Map[voterComplementInfo] == 0) {
      	if ( voterComplementInfo.VCIPrevName.length() > 0 || voterComplementInfo.VCIPrevName.length() > 0 || 
			        voterComplementInfo.VCIPrevAddress.length() > 0 || voterComplementInfo.VCIdataCountyId > 0 || voterComplementInfo.VCILastYearVote > 0 || 
      			  voterComplementInfo.VCILastDateVote > 0 || voterComplementInfo.VCIOtherParty.length() > 0 ) {

	        std::string tmpsql = "null,"; 
	        tmpsql += (voterComplementInfo.VotersId > 0) ? ("\"" + std::to_string(voterComplementInfo.VotersId) + "\",") : "null,";     
	        tmpsql += (voterComplementInfo.VCIPrevName.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(voterComplementInfo.VCIPrevName)) + "\",") : "null,";      
	        tmpsql += (voterComplementInfo.VCIPrevAddress.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(voterComplementInfo.VCIPrevAddress) + "\",") : "null,";      
	        tmpsql += (voterComplementInfo.VCIdataCountyId > 0) ? ("\"" + std::to_string(voterComplementInfo.VCIdataCountyId) + "\",") : "null,";     
	        tmpsql += (voterComplementInfo.VCILastYearVote > 0) ? ("\"" + std::to_string(voterComplementInfo.VCILastYearVote) + "\",") : "null,";     
	        tmpsql += (voterComplementInfo.VCILastDateVote > 0) ? ("\"" + std::to_string(voterComplementInfo.VCILastDateVote) + "\",") : "null,";     
	        tmpsql += (voterComplementInfo.VCIOtherParty.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(voterComplementInfo.VCIOtherParty) + "\"") : "null";      
	          
	        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
	        ++currentBatchCount;
	        SaveLast = true;
	      }	       
      	it = Map.erase(it);  // Erase and move to next element
    	} else {
	      ++it;  // Move to next element
	    }
	      
	 		if( currentBatchCount == batchSize) {
        sql = query + sql;
        SQL_INSERT
        sql.clear();
        currentBatchCount = 0;
        SaveLast = false;
      }
	  } 
	  
    if( SaveLast == true && currentBatchCount > 0) {        
      sql = query + sql;
      SQL_INSERT
      sql.clear();
    }
    
	} SQL_EXEPTION  
	CLOCK_END
	  
  sql.clear();
  sql = "SELECT * FROM VotersComplementInfo";
  if ( SimpleLastDbID[DBFIELDID_VOTERSCMINFO] > 0) {
    sql += " WHERE VotersComplementInfo >= " + std::to_string(SimpleLastDbID[DBFIELDID_VOTERSCMINFO]);    
  }
 
  executeLoadDataQuery(sql, Map);
  Map[VoterComplementInfo(NIL,NILSTRG,NILSTRG,NIL,NIL,NIL,NILSTRG)] = -2;
  return true;
}

void DataCollector::PrintTable(VoterComplementInfoMap& Map) {
	int Counter = 0;
	
	for(auto it = Map.begin(); it != Map.end(); ++it) {
		const VoterComplementInfo& compinfo = it->first;
	  int TableId = it->second;

  	std::cout << HI_WHITE << ++Counter << NC << " - Data ID: " << HI_PINK;

		//DataMailingAddress Data(mailingaddr.dataMailAdrL1, mailingaddr.dataMailAdrL2, mailingaddr.dataMailAdrL3, mailingaddr.dataMailAdrL4);
		//int value = Map[Data];
		
	  std::cout << "\tVoter ID:\t" << HI_YELLOW << compinfo.VotersId << NC << std::endl;
  	std::cout << "\tPrevious Name:\t" << HI_YELLOW << compinfo.VCIPrevName << NC << std::endl;
  	std::cout << "\tPrev Address:\t" << HI_YELLOW << compinfo.VCIPrevAddress << NC << std::endl;
  	std::cout << "\tCounty ID:\t" << HI_YELLOW << compinfo.VCIdataCountyId << NC << std::endl;
  	std::cout << "\tLast Year Voted:\t" << HI_YELLOW << compinfo.VCILastYearVote << NC << std::endl;
  	std::cout << "\tLast Vote:\t" << HI_YELLOW << compinfo.VCILastDateVote << NC << std::endl;
  	std::cout << "\tOther Party:\t" << HI_YELLOW << compinfo.VCIOtherParty << NC << std::endl;
		std::cout << std::endl;
	}
}


/**************************
 *** DATADISTRICT TABLE ***
 **************************
  Table: DataDistrict
    DataDistrict_ID int UN AI PK 
    DataCounty_ID int UN 
    DataDistrict_Electoral smallint UN 
    DataDistrict_StateAssembly smallint UN 
    DataDistrict_StateSenate tinyint UN 
    DataDistrict_Legislative smallint UN 
    DataDistrict_Ward char(3) 
    DataDistrict_Congress tinyint UN
*/


bool DataCollector::LoadData(DataDistrictMap& Map) {
  CHECK_FIELD
  executeLoadDataQuery("SELECT DataDistrict_ID, DataDistrict.DataCounty_ID, DataDistrict_Electoral, DataDistrict_StateAssembly, "  
                        "DataDistrict_StateSenate, DataDistrict_Legislative, DataDistrict_Ward, DataDistrict_Congress " 
                        "FROM RepMyBlockTwo.DataDistrict LEFT JOIN DataCounty ON "  
                        "(DataDistrict.DataCounty_ID = DataCounty.DataCounty_ID) " 
                        "WHERE DataState_ID = \"" + std::to_string(StateID) + "\"", Map);
  return true;
}

void DataCollector::executeLoadDataQuery(const std::string& sql, DataDistrictMap& Map) {
  CLOCK_START 
  try {     
    SQL_QUERY_START

    while (res->next()) {
      int index = res->getInt("DataDistrict_ID");
      
      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in LoadDataDistrict ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }

      Map[DataDistrict(
        SQL_INT_OR_NIL("DataCounty_ID"),  SQL_INT_OR_NIL("DataDistrict_Electoral"), SQL_INT_OR_NIL("DataDistrict_StateAssembly"),
        SQL_INT_OR_NIL("DataDistrict_StateSenate"), SQL_INT_OR_NIL("DataDistrict_Legislative"), SQL_UPPERSTR_OR_NIL("DataDistrict_Ward"),
        SQL_INT_OR_NIL("DataDistrict_Congress")
      )] = index;
    
      if ( index > SimpleLastDbID[DBFIELDID_DISTRICT]) { SimpleLastDbID[DBFIELDID_DISTRICT] = index; }
    }
        
    SQL_QUERY_END
  } SQL_EXEPTION  
  CLOCK_END
}


bool DataCollector::SaveDataBase(DataDistrictMap& Map) {  
	std::cout << HI_WHITE << "Saving DataDistrict" << NC << std::endl;
		
	const std::string query = "INSERT INTO DataDistrict VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  bool SaveLast = false;
  CLOCK_START

  try {
    auto it = Map.begin();
	  while (it != Map.end()) {
	  	    
      const DataDistrict& dataDistrict = it->first;
      if (Map[dataDistrict] == 0) {
      	if (dataDistrict.dataCountyId > 0 || dataDistrict.dataElectoral > 0 || dataDistrict.dataStateAssembly > 0 || dataDistrict.dataStateSenate > 0 || 
      			dataDistrict.dataLegislative > 0 || dataDistrict.dataWard.length() > 0 || dataDistrict.DataCongress > 0) {

	        // (`DataLastName_ID`, `DataFirstName_ID`, `DataMiddleName_ID`, `VotersIndexes_DOB`, `DataState_ID`, `VotersIndexes_UniqStateVoterID`)            
	        std::string tmpsql = "null,";         
	        tmpsql += (dataDistrict.dataCountyId > 0) ? ("\"" + std::to_string(dataDistrict.dataCountyId) + "\",") : "null,";     
	        tmpsql += (dataDistrict.dataElectoral > 0) ? ("\"" + std::to_string(dataDistrict.dataElectoral) + "\",") : "null,";     
	        tmpsql += (dataDistrict.dataStateAssembly > 0) ? ("\"" + std::to_string(dataDistrict.dataStateAssembly) + "\",") : "null,";     
	        tmpsql += (dataDistrict.dataStateSenate > 0) ? ("\"" + std::to_string(dataDistrict.dataStateSenate) + "\",") : "null,";     
	        tmpsql += (dataDistrict.dataLegislative > 0) ? ("\"" + std::to_string(dataDistrict.dataLegislative) + "\",") : "null,";     
	        tmpsql += (dataDistrict.dataWard.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataDistrict.dataWard)) + "\",") : "null,";      
	        tmpsql += (dataDistrict.DataCongress > 0) ? ("\"" + std::to_string(dataDistrict.DataCongress) + "\"") : "null";     
	     
	        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
	        ++currentBatchCount;
	        SaveLast = true;
        }
        it = Map.erase(it);
      } else {
     		++it;
      } 
      
      if( currentBatchCount == batchSize) {
        sql = query + sql;
        SQL_INSERT
        sql.clear();
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true && currentBatchCount > 0) {        
      sql = query + sql;
      SQL_INSERT
      sql.clear();
    }
    
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM DataDistrict";
  if ( SimpleLastDbID[DBFIELDID_DISTRICT] > 0) {
    sql += " WHERE DataDistrict_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_DISTRICT]);   
  }
  executeLoadDataQuery(sql, Map);
  Map[DataDistrict(NIL,NIL,NIL,NIL,NIL,NILSTRG,NIL)] = -2;
  return true;
}

/**********************************
 *** DATADISTRICTTEMPORAL TABLE ***
 **********************************
  Table: DataDistrictTemporal
  DataDistrictTemporal_ID int UN AI PK 
  DataDistrictCycle_ID int UN 
  DataHouse_ID int UN 
  DataDistrict_ID int UN
*/

bool DataCollector::LoadData(DataDistrictTemporalMap& Map) {
  CHECK_FIELD
  executeLoadDataQuery("SELECT * FROM DataDistrictTemporal", Map);
  return true;
}

bool DataCollector::SaveDataBase(DataDistrictTemporalMap& Map) {  
	
	std::cout << HI_WHITE << "Saving DataDistrictTemporal" << NC << std::endl;
		
	const std::string query = "INSERT INTO DataDistrictTemporal VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;
  CLOCK_START
  
  /*
    struct DataDistrictTemporal {
      int dataDistrictCycleId;
      int dataHouseId;
      int dataDistrictId;
  */
  
  
  try { 
    for(auto it = Map.begin(); it != Map.end(); ++it) {
      const DataDistrictTemporal& dataDistrictTemporal = it->first;
      int TableId = it->second;
        
      std::string Suffix;
    
      if (Map[dataDistrictTemporal] == 0 ) {
        // (`DataDistrictCycle_ID`, `DataHouse_ID`, `DataDistrict_ID`) 
                  
        // if (dataDistrictTemporal.dataHouseId > 0) {
        //    Suffix =  "\"" + dataDistrictTemporal.dataHouseId + "\"";
        // } else {
        //    Suffix = "null";          
        // }
        
        sql += ReturnDBInjest("(null, \"" + std::to_string(dataDistrictTemporal.dataDistrictCycleId) + "\",\"" + 
                              std::to_string(dataDistrictTemporal.dataDistrictCycleId) + "\",\"" + 
                              std::to_string(dataDistrictTemporal.dataDistrictCycleId) + 
                              + "\")", currentBatchCount);
        ++currentBatchCount;
        SaveLast = true;
        
      } else {
        std::cout << "We have an issue here" << std::endl;
        // std::cout << "Voter ID: " << Map[voterIdx] << "\tData Last Name ID: " << voterIdx.dataLastNameId << " - " << " Data First Name ID: " 
        //            << voterIdx.dataFirstNameId << " - " << " Data Middle Name ID: " << voterIdx.dataMiddleNameId << "\tData Name Suffix: " 
        //            << voterIdx.dataNameSuffix << " - " << " Data BOB: " << voterIdx.dataBOB << "\tData Uniq State ID: " << voterIdx.dataUniqStateId 
        //            << std::endl;
                    
        exit(1);
      }
      
      if( currentBatchCount == batchSize) {
        sql = query + sql;
        SQL_INSERT
        sql.clear();  
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true ) {
      sql = query + sql;
      SQL_INSERT
      sql.clear();  // Clearing the SQL string to start afresh for the next batch
    }   
  } SQL_EXEPTION  
  CLOCK_END
  
  return true;
}

void DataCollector::executeLoadDataQuery(const std::string& sql, DataDistrictTemporalMap& Map) {
  CLOCK_START
  try {     
    SQL_QUERY_START
   
    while (res->next()) {
      DataDistrictTemporal datadistricttemporal(
       SQL_INT_OR_NIL("DataDistrictCycle_ID"), SQL_INT_OR_NIL("DataHouse_ID"), SQL_INT_OR_NIL("DataDistrict_ID")
      );
      Map[datadistricttemporal] = res->getInt("DataDistrictTemporal_ID");
    }

    SQL_QUERY_END
  } SQL_EXEPTION
  CLOCK_END
}

/***********************
 *** DATAHOUSE TABLE ***
 ***********************
  Table: DataHouse
    DataHouse_ID int UN AI PK 
    DataAddress_ID int UN 
    DataHouse_Type varchar(10) 
    DataHouse_Apt varchar(100) 
    DataDistrictTown_ID int UN 
    DataStreetNonStdFormat_ID int UN 
    DataHouse_BIN int UN
*/


void DataCollector::executeLoadDataQuery(const std::string& sql, DataHouseMap& Map) {
  CLOCK_START
  try {     
    SQL_QUERY_START
 
    while (res->next()) {
      int index = res->getInt("DataHouse_ID");
      
      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in LoadDataHouse ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }

      Map[DataHouse(
        SQL_INT_OR_NIL("DataAddress_ID"), SQL_UPPERSTR_OR_NIL("DataHouse_Type"), SQL_UPPERSTR_OR_NIL("DataHouse_Apt"),
        SQL_INT_OR_NIL("DataDistrictTown_ID"), SQL_INT_OR_NIL("DataStreetNonStdFormat_ID"), SQL_INT_OR_NIL("DataHouse_BIN")
      )] = index;
    
      if ( index > SimpleLastDbID[DBFIELDID_HOUSE]) { SimpleLastDbID[DBFIELDID_HOUSE] = index; }
    }

    SQL_QUERY_END
  } SQL_EXEPTION
  CLOCK_END
}

bool DataCollector::LoadData(DataHouseMap& Map) { 
  CHECK_FIELD
  executeLoadDataQuery("SELECT * FROM DataHouse", Map);
  return true;
}

bool DataCollector::SaveDataBase(DataHouseMap& Map) {  
	std::cout << HI_WHITE << "Saving DataHouseMap" << NC << std::endl;
		
	const std::string query = "INSERT INTO DataHouse VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  bool SaveLast = false;
  CLOCK_START
 
  try {   
  	auto it = Map.begin();
    while (it != Map.end()) {
 		const DataHouse& dataHouse = it->first;
 		
      if (Map[dataHouse] == 0) {
      	if (dataHouse.dataAddressId > 0 || dataHouse.dataHouse_Type.length() > 0 || dataHouse.dataHouse_Apt.length() > 0 || 
      			dataHouse.dataDistrictTownId > 0 || dataHouse.dataStreetNonStdFormatId > 0) {      

	        std::string tmpsql = "null,";
	        tmpsql += (dataHouse.dataAddressId > 0) ? ("\"" + std::to_string(dataHouse.dataAddressId) + "\",") : "null,";     
	        tmpsql += (dataHouse.dataHouse_Type.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataHouse.dataHouse_Type)) + "\",") : "null,";      
	        tmpsql += (dataHouse.dataHouse_Apt.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataHouse.dataHouse_Apt)) + "\",") : "null,";      
	        tmpsql += (dataHouse.dataDistrictTownId > 0) ? ("\"" + std::to_string(dataHouse.dataDistrictTownId) + "\",") : "null,";        
	        tmpsql += (dataHouse.dataStreetNonStdFormatId > 0) ? ("\"" + std::to_string(dataHouse.dataStreetNonStdFormatId) + "\",") : "null,";     
	        tmpsql += "null";
	     		           
	        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
	        ++currentBatchCount;
	        SaveLast = true;
	      } 
	     	it = Map.erase(it);
      } else {
      	++it;
      }
      
      if( currentBatchCount == batchSize) {
        sql = query + sql;
        SQL_INSERT
        sql.clear();
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true && currentBatchCount > 0) {        
      sql = query + sql;
      SQL_INSERT
      sql.clear();
    }
   
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM DataHouse";
  if ( SimpleLastDbID[DBFIELDID_HOUSE] > 0) {
    sql += " WHERE DataHouse_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_HOUSE]);   
  }
 
  executeLoadDataQuery(sql, Map); 
  Map[DataHouse(NIL,NILSTRG,NILSTRG,NIL,NIL,NIL)] = -2;
  
  return true;
}


/*************************
 *** DATAADDRESS TABLE ***
 *************************
  Table: DataAddress
    DataAddress_ID int UN AI PK 
    DataAddress_HouseNumber varchar(100) 
    DataAddress_FracAddress varchar(100) 
    DataAddress_PreStreet varchar(100) 
    DataStreet_ID int UN 
    DataAddress_PostStreet varchar(100) 
    DataCity_ID int UN 
    DataCounty_ID int UN 
    DataAddress_zipcode varchar(30) 
    DataAddress_zip4 varchar(10) 
    Cordinate_ID int UN 
    PG_OSM_osmid bigint
*/

void DataCollector::executeLoadDataQuery(const std::string& sql, DataAddressMap& Map) {    
  CLOCK_START
  try {     
    SQL_QUERY_START

    while (res->next()) {                   
      int index = res->getInt("DataAddress_ID");  

      Map[DataAddress(
        SQL_UPPERSTR_OR_NIL("DataAddress_HouseNumber"), SQL_UPPERSTR_OR_NIL("DataAddress_FracAddress"),
        SQL_UPPERSTR_OR_NIL("DataAddress_PreStreet"), SQL_INT_OR_NIL("DataStreet_ID"), SQL_UPPERSTR_OR_NIL("DataAddress_PostStreet"),
        SQL_INT_OR_NIL("DataCity_ID"), SQL_INT_OR_NIL("DataCounty_ID"), SQL_UPPERSTR_OR_NIL("DataAddress_zipcode"),
        SQL_UPPERSTR_OR_NIL("DataAddress_zip4"), SQL_INT_OR_NIL("Cordinate_ID"), SQL_INT_OR_NIL("PG_OSM_osmid") 
      )] = index;
      
      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in DataAddressQuery ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }
      
      if ( index > SimpleLastDbID[DBFIELDID_ADDRESS]) { SimpleLastDbID[DBFIELDID_ADDRESS] = index; }        
    }

    SQL_QUERY_END
  } SQL_EXEPTION
   CLOCK_END 
}

bool DataCollector::LoadData(DataAddressMap& Map) {
  CHECK_FIELD
  std::cout << HI_WHITE << "Loading DataAddressMap" << NC << std::endl;
  executeLoadDataQuery("SELECT * FROM DataAddress", Map);
  return true;
}

bool DataCollector::SaveDataBase(DataAddressMap& Map) {  
	
	std::cout << HI_WHITE << "Saving DataAddressMap" << NC << std::endl;
		
	const std::string query = "INSERT INTO DataAddress VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;

  CLOCK_START

  try {   
   	auto it = Map.begin();
    while (it != Map.end()) {
      const DataAddress& dataAddress = it->first;
      int TableId = it->second;
          
      if ( Map[dataAddress] == 0) {
      	if (dataAddress.dataHouseNumber.length() > 0 || dataAddress.dataFracAddress.length() > 0 || 
	        dataAddress.dataPreStreet.length() > 0 || dataAddress.dataStreetId > 0 || dataAddress.dataPostStreet.length() > 0 || 
	        dataAddress.dataCityId > 0 || dataAddress.dataCityId > 0 || dataAddress.dataCountyId > 0 || dataAddress.dataZipcode.length() > 0 || 
	        dataAddress.dataZip4.length() > 0 ||  dataAddress.CordinateId > 0 || dataAddress.PGOSMosmid > 0) {
	          
	        std::string tmpsql = "null,";
	        tmpsql += (dataAddress.dataHouseNumber.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataHouseNumber)) + "\",") : "null,";      
	        tmpsql += (dataAddress.dataFracAddress.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataFracAddress)) + "\",") : "null,";      
	        tmpsql += (dataAddress.dataPreStreet.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataPreStreet)) + "\",") : "null,";
	        tmpsql += (dataAddress.dataStreetId > 0) ? ("\"" + std::to_string(dataAddress.dataStreetId) + "\",") : "null,";     
	        tmpsql += (dataAddress.dataPostStreet.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataPostStreet)) + "\",") : "null,";      
	        tmpsql += (dataAddress.dataCityId > 0) ? ("\"" + std::to_string(dataAddress.dataCityId) + "\",") : "null,";     
	        tmpsql += (dataAddress.dataCountyId > 0) ? ("\"" + std::to_string(dataAddress.dataCountyId) + "\",") : "null,";     
	        tmpsql += (dataAddress.dataZipcode.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataZipcode)) + "\",") : "null,";      
	        tmpsql += (dataAddress.dataZip4.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(ToUpperAccents(dataAddress.dataZip4)) + "\",") : "null,";      
	        tmpsql += (dataAddress.CordinateId > 0) ? ("\"" + std::to_string(dataAddress.CordinateId) + "\",") : "null,";     
	        tmpsql += (dataAddress.PGOSMosmid > 0) ? ("\"" + std::to_string(dataAddress.PGOSMosmid) + "\"") : "null";     
	       
	        sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
	        ++currentBatchCount;
	        SaveLast = true;
	      }
	      it = Map.erase(it);
      } else {
      	++it; 	
      }
    
 			if( currentBatchCount == batchSize) {
        sql = query + sql;
        SQL_INSERT
        sql.clear();
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true && currentBatchCount > 0) {        
      sql = query + sql;
      SQL_INSERT
      sql.clear();
    }
    
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM DataAddress";
  if ( SimpleLastDbID[DBFIELDID_ADDRESS] > 0) {
    sql += " WHERE DataAddress_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_ADDRESS]);   
  }
 
  executeLoadDataQuery(sql, Map);
  Map[DataAddress(NILSTRG,NILSTRG,NILSTRG,NIL,NILSTRG,NIL,NIL,NILSTRG,NILSTRG,NIL,NIL)] = -2;
   
  return true;
}

/********************************
 *** DATAMAILINGADDRESS TABLE ***
 ********************************
  Table: DataMailingAddress:
    DataMailingAddress_ID int UN AI PK 
    DataMailingAddress_Line1 varchar(256) 
    DataMailingAddress_Line2 varchar(256) 
    DataMailingAddress_Line3 varchar(256) 
    DataMailingAddress_Line4 varchar(256)
*/

void DataCollector::executeLoadDataQuery(const std::string& sql, DataMailingAddressMap& Map) {
  CLOCK_START  
  try {     
    SQL_QUERY_START

    while (res->next()) {
      int index = res->getInt("DataMailingAddress_ID");
        
      if ( index < 1 ) {
        std::cout << RED << "Problem with the data in DataMailingAddress ... Index is: " << NC << HI_YELLOW << index << NC << std::endl;
        exit(1);
      }   
         
      Map[DataMailingAddress(
      	simpleHash(res->getString("DataMailingAddress_Line1")),
        SQL_UPPERSTR_OR_NIL("DataMailingAddress_Line1"), SQL_UPPERSTR_OR_NIL("DataMailingAddress_Line2"),
        SQL_UPPERSTR_OR_NIL("DataMailingAddress_Line3"), SQL_UPPERSTR_OR_NIL("DataMailingAddress_Line4")
      )] = index;
                  
      if ( index > SimpleLastDbID[DBFIELDID_MAILADDRESS]) { SimpleLastDbID[DBFIELDID_MAILADDRESS] = index; }
      // PRINT_COUNTER
    }

    SQL_QUERY_END
  } SQL_EXEPTION  
  CLOCK_END  
}

bool DataCollector::LoadData(DataMailingAddressMap& Map) {
  CHECK_FIELD
  executeLoadDataQuery("SELECT * FROM DataMailingAddress", Map);
  return true;
}

bool DataCollector::SaveDataBase(DataMailingAddressMap& Map) {  
	const std::string query = "INSERT INTO DataMailingAddress VALUES ";
  std::string sql;
  int batchSize = SQLBATCH;
  int currentBatchCount = 0;
  int prev_size = 0;
  bool SaveLast = false;
  CLOCK_START

  try {     
    auto it = Map.begin();
    while (it != Map.end()) {
      const DataMailingAddress& DataMailingAddress = it->first;
              
      if ( Map[DataMailingAddress] == 0) {
      	if (DataMailingAddress.dataMailAdrL1.length() > 0 || DataMailingAddress.dataMailAdrL2.length() > 0 ||
          DataMailingAddress.dataMailAdrL3.length() > 0 || DataMailingAddress.dataMailAdrL4.length() > 0) {
            
        	// (`DataMailingAddress_Line1`, `DataMailingAddress_Line2`, `DataMailingAddress_Line3`, `DataMailingAddress_Line4`)
        	std::string tmpsql = "null,";
      
        	tmpsql += (DataMailingAddress.dataMailAdrL1.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(DataMailingAddress.dataMailAdrL1)) + "\",") : "null,";
        	tmpsql += (DataMailingAddress.dataMailAdrL2.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(DataMailingAddress.dataMailAdrL2)) + "\",") : "null,";
        	tmpsql += (DataMailingAddress.dataMailAdrL3.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(DataMailingAddress.dataMailAdrL3)) + "\",") : "null,";
        	tmpsql += (DataMailingAddress.dataMailAdrL4.length() > 0) ? ("\"" + dbConnection.CustomEscapeString(nameCase(DataMailingAddress.dataMailAdrL4)) + "\"") : "null";
     
        	sql += ReturnDBInjest("(" + tmpsql + ")", currentBatchCount);
       	 ++currentBatchCount;
	        SaveLast = true;  
      	}
       	it = Map.erase(it);
      } else {
      	++it; 	
      }
            
			// std::cout << HI_CYAN << "CURRENT BATCH COUNT: " << NC << HI_PINK << currentBatchCount << NC << std::endl;     
            
      if( currentBatchCount == batchSize) {
      	std::cout << HI_YELLOW << "WHAT IS THE SQL HERE IN BATCH SIZE: " << NC << HI_PINK << sql << NC << std::endl;   
        sql = query + sql;
        SQL_INSERT
        sql.clear();
        currentBatchCount = 0;
        SaveLast = false;
      }
    } 
  
    if( SaveLast == true && currentBatchCount > 0) {        
     	std::cout << HI_YELLOW << "WHAT IS THE SQL HERE IN LAST: " << NC << HI_PINK << sql << NC << std::endl;
      sql = query + sql;
      SQL_INSERT
      sql.clear();
    }
    
  } SQL_EXEPTION  
  CLOCK_END
  
  sql.clear();
  sql = "SELECT * FROM DataMailingAddress";
  if ( SimpleLastDbID[DBFIELDID_MAILADDRESS] > 0) {
    sql += " WHERE DataMailingAddress_ID >= " + std::to_string(SimpleLastDbID[DBFIELDID_MAILADDRESS]);    
  }
  
  executeLoadDataQuery(sql, Map);
  Map[DataMailingAddress(simpleHash(NILSTRG),NILSTRG,NILSTRG,NILSTRG,NILSTRG)] = -2;
  return true;
}

void DataCollector::PrintTable(DataMailingAddressMap& Map) {
	int Counter = 0;
	
	for(auto it = Map.begin(); it != Map.end(); ++it) {
		const DataMailingAddress& mailingaddr = it->first;
	  int TableId = it->second;

  	std::cout << HI_WHITE << ++Counter << NC << " - Data ID: " << HI_PINK;

		DataMailingAddress Data(NIL, mailingaddr.dataMailAdrL1, mailingaddr.dataMailAdrL2, mailingaddr.dataMailAdrL3, mailingaddr.dataMailAdrL4);
		int value = Map[Data];

  	std::cout << value << NC << std::endl;	  
	  std::cout << "\tMailing Line1:\t" << HI_YELLOW << mailingaddr.dataMailAdrL1 << NC << std::endl;
  	std::cout << "\tMailing Line2:\t" << HI_YELLOW << mailingaddr.dataMailAdrL2 << NC << std::endl;
  	std::cout << "\tMailing Line3:\t" << HI_YELLOW << mailingaddr.dataMailAdrL3 << NC << std::endl;
  	std::cout << "\tMailing Line4:\t" << HI_YELLOW << mailingaddr.dataMailAdrL4 << NC << std::endl;
		std::cout << std::endl;
	}
}

/***************************
 *** DATAFIRSTNAME TABLE ***
 ***************************
  Table: DataFirstName:
  DataFirstName_ID int UN AI PK 
  DataFirstName_Text varchar(256) 
  DataFirstName_Compress varchar(256)
*/


bool DataCollector::LoadFirstName(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_FIRSTNAME), DBFIELDID_FIRSTNAME); 	
  return true;
}

bool DataCollector::TriggerSaveFirstNameDB(void) {
  executeSimpleSave(DBFIELDID_FIRSTNAME);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_FIRSTNAME) + " WHERE " + std::string(DBFIELD_FIRSTNAME) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_FIRSTNAME]), DBFIELDID_FIRSTNAME);  
  return true;
}

/**************************
 *** DATALASTNAME TABLE ***
 **************************
  Table: DataLastName:
    DataLastName_ID int UN AI PK 
    DataLastName_Text varchar(256) 
    DataLastName_Compress varchar(256)
*/

bool DataCollector::LoadLastName(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_LASTNAME), DBFIELDID_LASTNAME);     
  return true;
}

bool DataCollector::TriggerSaveLastNameDB(void) {
  executeSimpleSave(DBFIELDID_LASTNAME);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_LASTNAME) + " WHERE " + std::string(DBFIELD_LASTNAME) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_LASTNAME]), DBFIELDID_LASTNAME);
  return true;
}

/****************************
 *** DATAMIDDLENAME TABLE ***
 ****************************
  Table: DataMiddleName
    DataMiddleName_ID int UN AI PK 
    DataMiddleName_Text varchar(256) 
    DataMiddleName_Compress varchar(256)
*/

bool DataCollector::LoadMiddleName(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_MIDDLENAME), DBFIELDID_MIDDLENAME);
  return true;
}

bool DataCollector::TriggerSaveMiddleNameDB(void) {
  executeSimpleSave(DBFIELDID_MIDDLENAME);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_MIDDLENAME) + " WHERE " + std::string(DBFIELD_MIDDLENAME) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_MIDDLENAME]), DBFIELDID_MIDDLENAME);
  return true;
}

/***********************
 *** DATASTATE TABLE ***
 ***********************
  Table: DataState
    DataState_ID int UN AI PK 
    DataState_Name varchar(255) 
    DataState_Abbrev char(2)
*/

bool DataCollector::LoadStateName(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_STATENAME), DBFIELDID_STATENAME);     
  return true;
}

bool DataCollector::LoadStateAbbrev(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_STATEABBREV), DBFIELDID_STATEABBREV);
  return true;
}

/*

int DataCollector::LoadStateAbbrev(const std::string& StateAbbrev) {
  CHECK_FIELD
  std::string sql = "SELECT * FROM " + std::string(DBFIELD_STATEABBREV) + " WHERE DataState_Abbrev = \"" + dbConnection.CustomEscapeString(StateAbbrev) + "\"";

  int index;
  try {
    SQL_QUERY_START   
    res->next();
    index = res->getInt(std::string(DBFIELD_STATEABBREV) + "_ID");
    SQL_QUERY_END     
  } SQL_EXEPTION  
  return index;
}

/************************
 *** DATASTREET TABLE ***
 ************************
  Table: DataStreet
    DataStreet_ID int UN AI PK 
    DataStreet_Name varchar(255)
*/

bool DataCollector::LoadStreetName(VoterMap& voterMap) {  
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_STREET), DBFIELDID_STREET);
  return true;
}

bool DataCollector::TriggerSaveStreetNameDB(void) {
  executeSimpleSave(DBFIELDID_STREET);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_STREET) + " WHERE " + std::string(DBFIELD_STREET) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_STREET]), DBFIELDID_STREET);
  return true;
}

/******************************
 *** DATADISTRICTTOWN TABLE ***
 ******************************
  Table: DataDistrictTown
    DataDistrictTown_ID int UN AI PK 
    DataDistrictTown_Name varchar(255)
*/

bool DataCollector::LoadDistrictTown(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_DISTRICTTOWN), DBFIELDID_DISTRICTTOWN);
  return true;
}


bool DataCollector::TriggerSaveDistrictTownDB(void) {
  executeSimpleSave(DBFIELDID_DISTRICTTOWN);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_DISTRICTTOWN) + " WHERE " + std::string(DBFIELD_DISTRICTTOWN) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_DISTRICTTOWN]), DBFIELDID_DISTRICTTOWN);  
  return true;
}

/**********************
 *** DATACITY TABLE ***
 **********************
  Table: DataCity
    DataCity_ID int UN AI PK 
    DataCity_Name varchar(255)
*/

bool DataCollector::LoadCity(VoterMap& voterMap) {  
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_CITY), DBFIELDID_CITY);     
  return true;
}

bool DataCollector::TriggerSaveCityDB(void) {
  executeSimpleSave(DBFIELDID_CITY);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_CITY) + " WHERE " + std::string(DBFIELD_CITY) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_CITY]), DBFIELDID_CITY);  
  return true;
}

/************************************
 *** DATASTREETNONSTDFORMAT TABLE ***
 ************************************
  Table: DataStreetNonStdFormat
    DataStreetNonStdFormat_ID int UN AI PK 
    DataStreetNonStdFormat_Text varchar(250)
*/

bool DataCollector::LoadNonStdFormat(VoterMap& voterMap) {
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_NONSTDFORMAT), DBFIELDID_NONSTDFORMAT);     
  return true;
}

bool DataCollector::TriggerSaveNonStdFormatDB(void) {
  executeSimpleSave(DBFIELDID_NONSTDFORMAT);
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_NONSTDFORMAT) + " WHERE " + std::string(DBFIELD_NONSTDFORMAT) + "_ID >= " + 
                      std::to_string(SimpleLastDbID[DBFIELDID_NONSTDFORMAT]), DBFIELDID_NONSTDFORMAT);  
  return true;
}

/************************
 *** DATACOUNTY TABLE ***
 ************************
  Table: DataCounty
    DataCounty_ID int UN AI PK 
    DataState_ID int UN 
    DataCounty_Name varchar(40) 
    DataCounty_BOEID int UN
*/

bool DataCollector::LoadCounty(VoterMap& voterMap) {  
  CHECK_FIELD
  executeSimpleQuery("SELECT * FROM " + std::string(DBFIELD_COUNTY) + " WHERE DataState_ID = \"" + std::to_string(StateID) + "\"", DBFIELDID_COUNTY);     
  return true;
}

/***************************************
 * ALL THE DATA MANIPULATION FUNCTIONS *
 ***************************************/

// String Manipulation function for the database
std::string DataCollector::intToMySQLDate(int dateInt) {
  if (dateInt > 0) {
    int year = dateInt / 10000;
    int month = (dateInt / 100) % 100;
    int day = dateInt % 100;

    std::stringstream ss;
    ss  << std::setw(4) << std::setfill('0') << year << '-'
        << std::setw(2) << std::setfill('0') << month << '-'
        << std::setw(2) << std::setfill('0') << day;
    return ss.str();
  }
  return NILSTRG;
}

int DataCollector::mysqlDateToInt(const std::string& mysqlDate) {
  if (mysqlDate.size() != 10 || mysqlDate[4] != '-' || mysqlDate[7] != '-') {
    // std::cerr << "Invalid MySQL date format." << std::endl;
    return NIL;  // or throw an exception, or handle it in some other appropriate way
  }
  
  int year, month, day;
  sscanf(mysqlDate.c_str(), "%d-%d-%d", &year, &month, &day);
  return year * 10000 + month * 100 + day;
}

std::string DataCollector::ToUpperAccents(const std::string& input) {
  std::string result;
  
  for (unsigned char c : input) {
    if (isalpha(c)) { 
      try {
        result += toupper(static_cast<unsigned char>(c));
      } catch (const std::exception& e) {
        std::cerr << "Exception: " << c << " One: " << e.what() << std::endl;
        exit(1);
      }
    } else {
      switch (c) {    
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:   // àáâãäå
        case 0xA8: case 0xA9: case 0xAA: case 0xAB:                         // èéêë
        case 0xAC: case 0xAD: case 0xAE: case 0xAF:                         // ìíîï
        case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8:   // òóôõöø
        case 0xB9: case 0xBA: case 0xBB: case 0xBC:                         // ùúûü
        case 0xA7: case 0xB1:                                               // çñ
          result += (c - 32);
          break;
        
        default: case 0x9F: case 0xC3:// ß
          result += c;
          break;          
      }
    }
  }
  return result;
}

std::string DataCollector::ToLowerAccents(const std::string& input) {
  std::string result;
  
  for (unsigned char c : input) {
    if (isalpha(c)) { 
      try {
        result += tolower(static_cast<unsigned char>(c));
      } catch (const std::exception& e) {
        std::cerr << "Exception: " << c << " One: " << e.what() << std::endl;
        exit(1);
      }
    } else {
      switch (c) {    
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85:   // ÀÁÂÃÄÅ
        case 0x88: case 0x89: case 0x8A: case 0x8B:                         // ÈÉÊË
        case 0x8C: case 0x8D: case 0x8E: case 0x8F:                         // ÌÍÎÏ
        case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x98:   // ÒÓÔÕÖØ
        case 0x99: case 0x9A: case 0x9B: case 0x9C:                         // ÙÚÛÜ
        case 0x87: case 0x91:                                               // ÇÑ
          result += (c + 32);
          break;
        
        default: case 0x9F: case 0xC3: // ß
          result += c;
          break;
      }
    }
  }
  return result;
}

void DataCollector::exitIfSequenceFound(const std::string& str, const std::string sequenceToFind) {
  
  // Search for the sequence
  if (str.find(sequenceToFind) != std::string::npos) {
    // If found, print message and exit
    std::cerr << "Byte sequence " << sequenceToFind << " found. Exiting program.\n";
    std::exit(EXIT_FAILURE);
  }
}



std::string DataCollector::nameCase(const std::string& input) {
  std::string result;
  bool capitalizeNext = true;
  
  for (unsigned char c : input) {
    if (capitalizeNext) {
      if (isalpha(c)) { 
        try {
          result += toupper(static_cast<unsigned char>(c));
        } catch (const std::exception& e) {
          std::cerr << "Exception: " << c << " One: " << e.what() << std::endl;
          exit(1);
        }
      } else {
        switch (c) {    
          case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:   // àáâãäå
          case 0xA8: case 0xA9: case 0xAA: case 0xAB:                         // èéêë
          case 0xAC: case 0xAD: case 0xAE: case 0xAF:                         // ìíîï
          case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8:   // òóôõöø
          case 0xB9: case 0xBA: case 0xBB: case 0xBC:                         // ùúûü
          case 0xA7: case 0xB1:                                               // çñ
            result += (c - 32);
            break;
          
          default: case 0x9F: case 0xC3:  // ß
            result += c;
            break;
        }
      }
      capitalizeNext = false;
      
    } else {
      if (isalpha(c)) { 
        try {
          result += tolower(static_cast<unsigned char>(c));
        } catch (const std::exception& e) {
          std::cerr << "Exception: " << c << " Two: " << e.what() << std::endl;
          exit(1);
        }
      } else {
        switch (c) {
          case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85:   // ÀÁÂÃÄÅ
          case 0x88: case 0x89: case 0x8A: case 0x8B:                         // ÈÉÊË
          case 0x8C: case 0x8D: case 0x8E: case 0x8F:                         // ÌÍÎÏ
          case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x98:   // ÒÓÔÕÖØ
          case 0x99: case 0x9A: case 0x9B: case 0x9C:                         // ÙÚÛÜ
          case 0x87: case 0x91:                                               // ÇÑ
            result += (c + 32);
            break;
              
          default: case 0x9F: case 0xC3:  // ß
            result += c;
            break;
        }   
      }
      
    }
    // If the character is a space, capitalize the next character
    if (c == ' ') { capitalizeNext = true; }   
  }
  return result;
}

std::string DataCollector::RemoveAllSpacesString(const std::string& name) {
  std::string cleanedName;
  std::copy_if(name.begin(), name.end(), std::back_inserter(cleanedName), [](unsigned char c) {
    return std::isalnum(c) || (c == 0xC3 || ( c >= 0x80 && c <= 0xBC) || c >= 192);  // keeps alphanumeric and accented characters
  });
  
  return ToUpperAccents(cleanedName);
}





// Special Fields in the database 
bool DataCollector::stringToBool(const std::string& str) {
  if (str == "yes") return true;
  if (str == "no") return false;
  if (str == "Y") return true;
  if (str == "N") return false;
     
  std::cout << HI_RED << "String to String To Bool coulnd't be returned: " << str << NC << std::endl;
  exit(1);
}

std::string DataCollector::boolToString(bool source) {
  if (source == true) return "yes";
  if (source == false) return "no";
  return NILSTRG;
}

std::string DataCollector::genderToString(Gender gender) {
  switch (gender) {
    case Gender::Male: return "Male";
    case Gender::Female: return "Female";
    case Gender::Other: return "Other";
    case Gender::Undetermined: return "Undetermined";
    case Gender::Unspecified: return "Unspecified";
    default: return NILSTRG;
  }
}

Gender DataCollector::stringToGender(const std::string& str) {
  
  if (str == "MALE") return Gender::Male;
  if (str == "M") return Gender::Male;
  if (str == "FEMALE") return Gender::Female;
  if (str == "F") return Gender::Female;
  if (str == "OTHER") return Gender::Other;
  if (str == "U") return Gender::Undisclosed;
  if (str == "X") return Gender::Undisclosed;
  if (str == "I") return Gender::Intersex;
  if (str == "UNDERTERMINED") return Gender::Undetermined;
  if (str == "UNSPECIFIED") return Gender::Unspecified;

  std::cout << HI_RED << "String to Gender coulnd't be changed: " << str << NC << std::endl;
  exit(1);
  //  throw std::invalid_argument("Unknown Gender string");
}

std::string DataCollector::reasonCodeToString(ReasonCode code) {
  switch (code) {
    case ReasonCode::AdjudgedIncompetent: return "AdjudgedIncompetent";
    case ReasonCode::Death: return "Death";
    case ReasonCode::Duplicate: return "Duplicate";
    case ReasonCode::Felon: return "Felon";
    case ReasonCode::MailCheck: return "MailCheck";
    case ReasonCode::MovedOutCounty: return "MovedOutCounty";
    case ReasonCode::NCOA: return "NCOA";
    case ReasonCode::NVRA: return "NVRA";
    case ReasonCode::ReturnMail: return "ReturnMail";
    case ReasonCode::VoterRequest: return "VoterRequest";
    case ReasonCode::Other: return "Other";
    case ReasonCode::Court: return "Court";
    case ReasonCode::Inactive: return "Inactive";
    case ReasonCode::Unspecified: return "Unspecified";
    default: return NILSTRG;
  }
}

ReasonCode DataCollector::stringToReasonCode(const std::string& str) {
  if (str == "AdjudgedIncompetent") return ReasonCode::AdjudgedIncompetent;
  if (str == "Death") return ReasonCode::Death;
  if (str == "Duplicate") return ReasonCode::Duplicate;
  if (str == "Felon") return ReasonCode::Felon;
  if (str == "MailCheck") return ReasonCode::MailCheck;
  if (str == "MovedOutCounty") return ReasonCode::MovedOutCounty;
  if (str == "NCOA") return ReasonCode::NCOA;
  if (str == "NVRA") return ReasonCode::NVRA;
  if (str == "ReturnMail") return ReasonCode::ReturnMail;
  if (str == "VoterRequest") return ReasonCode::VoterRequest;
  if (str == "Other") return ReasonCode::Other;
  if (str == "Court") return ReasonCode::Court;
  if (str == "Inactive") return ReasonCode::Inactive;
  if (str == "Unspecified") return ReasonCode::Unspecified;
  if (str == "DEATH") return ReasonCode::Death;
  if (str == "ADJ-INCOMP") return ReasonCode::AdjudgedIncompetent;
  if (str == "DUPLICATE") return ReasonCode::Duplicate;
  if (str == "FELON") return ReasonCode::Felon;
  if (str == "MAIL-CHECK") return ReasonCode::MailCheck;
  if (str == "MOVED") return ReasonCode::MovedOutCounty;
  if (str == "RETURN-MAIL") return ReasonCode::ReturnMail;
  if (str == "VOTER-REQ") return ReasonCode::VoterRequest;
  return ReasonCode::Undefined;
}

std::string DataCollector::statusToString(Status status) {
  switch (status) {
    case Status::Active: return "Active";
    case Status::ActiveMilitary: return "ActiveMilitary";
    case Status::ActiveSpecialFederal: return "ActiveSpecialFederal";
    case Status::ActiveSpecialPresidential: return "ActiveSpecialPresidential";
    case Status::ActiveUOCAVA: return "ActiveUOCAVA";
    case Status::Inactive: return "Inactive";
    case Status::Purged: return "Purged";
    case Status::Prereg17YearOlds: return "Prereg17YearOlds";
    case Status::Confirmation: return "Confirmation";
    case Status::Unspecified: return "Unspecified";
    default: return NILSTRG;
  }
}

Status DataCollector::stringToStatus(const std::string& str) {
  if (str == "Active") return Status::Active;
  if (str == "A") return Status::Active;
  if (str == "ActiveMilitary") return Status::ActiveMilitary;
  if (str == "ActiveSpecialFederal") return Status::ActiveSpecialFederal;
  if (str == "ActiveSpecialPresidential") return Status::ActiveSpecialPresidential;
  if (str == "ActiveUOCAVA") return Status::ActiveUOCAVA;
  if (str == "Inactive") return Status::Inactive;
  if (str == "Purged") return Status::Purged;
  if (str == "Prereg17YearOlds") return Status::Prereg17YearOlds;
  if (str == "Confirmation") return Status::Confirmation;
  if (str == "Unspecified") return Status::Unspecified;
    
  if (str == "AM") return Status::ActiveMilitary;
  if (str == "AF") return Status::ActiveSpecialFederal;
  if (str == "AP") return Status::ActiveSpecialPresidential;
  if (str == "AU") return Status::ActiveUOCAVA;
  if (str == "I") return Status::Inactive;
  if (str == "P") return Status::Purged;
  if (str == "17") return Status::Prereg17YearOlds;
  if (str == "Confirmation") return Status::Confirmation;
  if (str == "Unspecified") return Status::Unspecified;
  
  std::cout << HI_RED << "String to Status coulnd't be returned: " << str << NC << std::endl;
  exit(1);
}

std::string DataCollector::regSourceToString(RegSource source) {
  switch (source) {
    case RegSource::Agency: return "Agency";
    case RegSource::CBOE: return "CBOE";
    case RegSource::DMV: return "DMV";
    case RegSource::LocalRegistrar: return "LocalRegistrar";
    case RegSource::MailIn: return "MailIn";
    case RegSource::School: return "School";
    case RegSource::Unspecified: return "Unspecified";
    case RegSource::OVR: return "OVR";
    default: return NILSTRG;
  }
}

RegSource DataCollector::stringToRegSource(const std::string& str) {
  if (str == "Agency") return RegSource::Agency;
  if (str == "CBOE") return RegSource::CBOE;
  if (str == "DMV") return RegSource::DMV;
  if (str == "LocalRegistrar") return RegSource::LocalRegistrar;
  if (str == "MailIn") return RegSource::MailIn;
  if (str == "School") return RegSource::School;
  if (str == "Unspecified") return RegSource::Unspecified;
  if (str == "AGCY") return RegSource::Agency;
  if (str == "LOCALREG") return RegSource::LocalRegistrar;
  if (str == "MAIL") return RegSource::MailIn;
  if (str == "SCHOOL") return RegSource::School;
  if (str == "OVR") return RegSource::OVR;
  if (str == "Unspecified") return RegSource::Unspecified;
     
  std::cout << HI_RED << "String to Reg Source coulnd't be returned: " << str << NC << std::endl;
  exit(1);
}

// Function to left-rotate a 32-bit integer
inline uint32_t DataCollector::leftRotate(uint32_t x, uint32_t n) {
	return (x << n) | (x >> (32 - n));
}

// MD5 main function

uint32_t DataCollector::simpleHash(const std::string& inputin) {

	std::string input = RemoveAllSpacesString(inputin);

	// Constants for MD5 algorithm
	constexpr uint32_t S[64] = {
	  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	  5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
	  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	constexpr uint32_t K[64] = {
	  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

  uint32_t a0 = 0x67452301;
  uint32_t b0 = 0xefcdab89;
  uint32_t c0 = 0x98badcfe;
  uint32_t d0 = 0x10325476;
  
  std::vector<uint8_t> binaryInput(input.begin(), input.end());
 	binaryInput.push_back(0x80);

	while ((binaryInput.size() * 8) % 512 != 448) {
    binaryInput.push_back(0);
  }

	uint64_t originalLengthBits = static_cast<uint64_t>(input.length()) * 8;
	for (int i = 0; i < 8; ++i) {
	    binaryInput.push_back((originalLengthBits >> (i * 8)) & 0xFF);
	}
	
	for (size_t i = 0; i < binaryInput.size(); i += 64) {
	  uint32_t M[16];
	  for (int j = 0; j < 16; ++j) {
	    M[j] = (binaryInput[i + j*4]) | (binaryInput[i + j*4 + 1] << 8) |
	           (binaryInput[i + j*4 + 2] << 16) | (binaryInput[i + j*4 + 3] << 24);
	  }

	  uint32_t A = a0, B = b0, C = c0, D = d0;
	  
	  // Main loop
	  for (int j = 0; j < 64; ++j) {
			uint32_t F, g;

			if (j < 16) {
				F = (B & C) | ((~B) & D);
				g = j;
			} else if (j < 32) {
				F = (D & B) | ((~D) & C);
				g = (5 * j + 1) % 16;
			} else if (j < 48) {
				F = B ^ C ^ D;
				g = (3 * j + 5) % 16;
			} else {
				F = C ^ (B | (~D));
				g = (7 * j) % 16;
			}

			F = F + A + K[j] + M[g];
			A = D;
			D = C;
			C = B;
			B = B + leftRotate(F, S[j]);
	  }

	  // Update the hash values
	  a0 += A; b0 += B; c0 += C; d0 += D;
	}

	uint32_t result = (a0 & 0xFF) | ((b0 & 0xFF) << 8) | ((c0 & 0xFF) << 16) | ((d0 & 0xFF) << 24);
  return result;
}

std::string DataCollector::uintToString(uint32_t value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}
