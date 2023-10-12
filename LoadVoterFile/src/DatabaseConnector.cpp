#include "DatabaseConnector.h"
#include "Voter.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>  // Include this header for timing

DatabaseConnector::DatabaseConnector() : con(nullptr) {}

DatabaseConnector::~DatabaseConnector() {
	if(con != nullptr) {
	  delete con;
	}
}

bool DatabaseConnector::connect(VoterMap& voterMap) {
	try {
	  sql::mysql::MySQL_Driver* driver;
	  driver = sql::mysql::get_mysql_driver_instance();
	  con = driver->connect(DB_HOST + ":" + std::to_string(DB_PORT), DB_USER, DB_PASS);
	  con->setSchema(DB_NAME);

	  std::cout << "Connected to MySQL server." << std::endl;
	  std::cout << "I am in the connector" << std::endl;
	  	
	  return true;
	} catch (sql::SQLException &e) {
	  std::cerr << "Error: Could not connect to database. " << e.what() << std::endl;
	  return false;
	}
}

bool DatabaseConnector::connect(VoterIdxMap& voterIdxMap) {
	try {
	  sql::mysql::MySQL_Driver* driver;
	  driver = sql::mysql::get_mysql_driver_instance();
	  con = driver->connect(DB_HOST + ":" + std::to_string(DB_PORT), DB_USER, DB_PASS);
	  con->setSchema(DB_NAME);

	  std::cout << "Connected to MySQL server." << std::endl;
	  std::cout << "I am in the connector" << std::endl;
	  	
	  return true;
	} catch (sql::SQLException &e) {
	  std::cerr << "Error: Could not connect to database. " << e.what() << std::endl;
	  return false;
	}
}

int DatabaseConnector::ReturnIndex(const std::string& query) {
	if (dataMap[query] == 0 ) {	dataMap[query] = -1; }
	return dataMap[query];
}

bool DatabaseConnector::LoadFirstName(VoterMap& voterMap) {
	if (dbFieldType > -1) { 
		std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
		return false;
	}

	executeSimpleQuery("SELECT * FROM " + DBFIELD_FIRSTNAME, DBFIELDID_FIRSTNAME);
	return true;
}

bool DatabaseConnector::LoadLastName(VoterMap& voterMap) {
	if (dbFieldType > -1) { 
		std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
		return false;
	}
	
  executeSimpleQuery("SELECT * FROM " + DBFIELD_LASTNAME, DBFIELDID_LASTNAME);			
	return true;
}

bool DatabaseConnector::LoadMiddleName(VoterMap& voterMap) {
	
	if (dbFieldType > -1) { 
		std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
		return false;
	}
	
  executeSimpleQuery("SELECT * FROM " + DBFIELD_MIDDLENAME, DBFIELDID_MIDDLENAME);
	return true;
}

bool DatabaseConnector::LoadStateName(VoterMap& voterMap) {
	
	if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
  
    executeSimpleQuery("SELECT * FROM " + DBFIELD_STATENAME, DBFIELDID_STATENAME);			
		return true;
}

bool DatabaseConnector::LoadStateAbbrev(VoterMap& voterMap) {
	
		if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
    executeSimpleQuery("SELECT * FROM " + DBFIELD_STATEABBREV, DBFIELDID_STATEABBREV);
		return true;
}

bool DatabaseConnector::LoadStreetName(VoterMap& voterMap) {
	
	if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
    executeSimpleQuery("SELECT * FROM " + DBFIELD_STREET, DBFIELDID_STREET);			
		return true;
}

bool DatabaseConnector::LoadDistrictTown(VoterMap& voterMap) {
	
		if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
 
    executeSimpleQuery("SELECT * FROM " + DBFIELD_DISTRICTTOWN, DBFIELDID_DISTRICTTOWN);
		return true;
}

bool DatabaseConnector::LoadCity(VoterMap& voterMap) {
	
	if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
  
    executeSimpleQuery("SELECT * FROM " + DBFIELD_CITY, DBFIELDID_CITY);			
		return true;
}

bool DatabaseConnector::LoadNonStdFormat(VoterMap& voterMap) {
	
	if (dbFieldType > -1) { 
			std::cerr << "DB Field Type is set for " << dbFieldType << " and can't be used anymore" << std::endl;
			return false;
		}
  
    executeSimpleQuery("SELECT * FROM " + DBFIELD_NONSTDFORMAT, DBFIELDID_NONSTDFORMAT);			
		return true;
}

void DatabaseConnector::executeSimpleQuery(const std::string& query, int DBCount) {

	try {    	
		auto start = std::chrono::high_resolution_clock::now();
	
    sql::Statement* stmt;
    sql::ResultSet* res;

    stmt = con->createStatement();
    res = stmt->executeQuery(query);

		// std::map<std::string, int> dataMap;
	
    while (res->next()) {    	
    	std::string idName, textName;
	   	switch (DBCount) {
  			case DBFIELDID_STATENAME:			idName = DBFIELD_STATENAME + "_ID"; 		textName = DBFIELD_STATENAME + "_Name"; 		break;
				case DBFIELDID_STATEABBREV:		idName = DBFIELD_STATEABBREV + "_ID"; 	textName = DBFIELD_STATEABBREV + "_Abbrev"; break;
				case DBFIELDID_STREET:				idName = DBFIELD_STREET + "_ID"; 				textName = DBFIELD_STREET + "_Name"; 				break;
				case DBFIELDID_MIDDLENAME: 		idName = DBFIELD_MIDDLENAME + "_ID"; 		textName = DBFIELD_MIDDLENAME + "_Text"; 		break;
				case DBFIELDID_LASTNAME: 			idName = DBFIELD_LASTNAME + "_ID"; 			textName = DBFIELD_LASTNAME + "_Text"; 			break;
				case DBFIELDID_FIRSTNAME:			idName = DBFIELD_FIRSTNAME + "_ID"; 		textName = DBFIELD_FIRSTNAME + "_Text"; 		break;
				case DBFIELDID_DISTRICTTOWN:	idName = DBFIELD_DISTRICTTOWN + "_ID"; 	textName = DBFIELD_DISTRICTTOWN + "_Name"; 	break;
				case DBFIELDID_CITY:					idName = DBFIELD_CITY + "_ID"; 					textName = DBFIELD_CITY + "_Name"; 					break;
				case DBFIELDID_NONSTDFORMAT:	idName = DBFIELD_NONSTDFORMAT + "_ID"; 	textName = DBFIELD_NONSTDFORMAT + "_Text"; 	break;
  		}

	    int index = res->getInt(idName);
	    std::string name = ToUpperAccents(res->getString(textName));
	    dataMap[name] = index;
  	}
  	
    delete res;
    delete stmt;
    
    // Record the end time
    auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			
    std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;
    std::cout << "Query executed successfully." << std::endl;
    	
  } catch (sql::SQLException &e) {
      std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
  }

}

// These are the comp
bool DatabaseConnector::LoadVoters(VoterMap& Map) {
    executeLoadVotersQuery("SELECT * FROM Voters WHERE DataState_ID = 1", Map);
    return true;
}

bool DatabaseConnector::LoadVotersIdx(VoterIdxMap& Map) {
    executeLoadVotersIdxQuery("SELECT * FROM VotersIndexes WHERE DataState_ID = 1", Map);
    return true;
}

bool DatabaseConnector::LoadVotersComplementInfo(VoterComplementInfoMap& Map) {
    executeLoadVotersComplementInfoQuery("SELECT * FROM VotersComplementInfo", Map);
    return true;
}


bool DatabaseConnector::LoadDataMailingAddress(DataMailingAddressMap& Map) {
    executeLoadDataMailingAddressQuery("SELECT * FROM DataMailingAddress", Map);
    return true;
}

bool DatabaseConnector::LoadDataDistrict(DataDistrictMap& Map) {
    executeLoadDataDistrictQuery("SELECT * FROM DataDistrict", Map);
    return true;
}

bool DatabaseConnector::LoadDataDistrictTemporal(DataDistrictTemporalMap& Map) {
    executeLoadDataDistrictTemporalQuery("SELECT * FROM DataDistrictTemporal", Map);
    return true;
}

bool DatabaseConnector::LoadDataHouse(DataHouseMap& Map) { 
    executeLoadDataHouseQuery("SELECT * FROM DataHouse", Map);
    return true;
}

bool DatabaseConnector::LoadDataAddress(DataAddressMap& Map) {
    executeLoadDataAddressQuery("SELECT * FROM DataAddress", Map);
    return true;
}


void DatabaseConnector::executeLoadVotersQuery(const std::string& query, VoterMap& Map) {
	
	std::cout << "DatabaseConnector::executeSpecificQuery" << std::endl;
	std::cout << "Executing the query: " << query << std::endl;

	int Counter = 0;

	try {    	
		auto start = std::chrono::high_resolution_clock::now();

		sql::Statement* stmt;
		sql::ResultSet* res;

		stmt = con->createStatement();
		res = stmt->executeQuery(query);

		while (res->next()) {
			Voter voter(
				res->getInt("VotersIndexes_ID"),
				res->getInt("DataHouse_ID"),

				res->getString("Voters_Gender") == "male" ? Gender::Male :
				res->getString("Voters_Gender") == "female" ? Gender::Female :
				res->getString("Voters_Gender") == "other" ? Gender::Other :
				res->getString("Voters_Gender") == "undetermined" ? Gender::Undetermined : Gender::Unspecified,

				res->getString("Voters_UniqStateVoterID"),
				res->getString("Voters_RegParty"),

				res->getString("Voters_ReasonCode") == "AdjudgedIncompetent" ? ReasonCode::AdjudgedIncompetent :
				res->getString("Voters_ReasonCode") == "Death" ? ReasonCode::Death :
				res->getString("Voters_ReasonCode") == "Duplicate" ? ReasonCode::Duplicate :
				res->getString("Voters_ReasonCode") == "Felon" ? ReasonCode::Felon :
				res->getString("Voters_ReasonCode") == "MailCheck" ? ReasonCode::MailCheck :
				res->getString("Voters_ReasonCode") == "MovedOutCounty" ? ReasonCode::MovedOutCounty :
				res->getString("Voters_ReasonCode") == "NCOA" ? ReasonCode::NCOA :
				res->getString("Voters_ReasonCode") == "NVRA" ? ReasonCode::NVRA :
				res->getString("Voters_ReasonCode") == "ReturnMail" ? ReasonCode::ReturnMail :
				res->getString("Voters_ReasonCode") == "VoterRequest" ? ReasonCode::VoterRequest :
				res->getString("Voters_ReasonCode") == "Other" ? ReasonCode::Other :
				res->getString("Voters_ReasonCode") == "Court" ? ReasonCode::Court :
				res->getString("Voters_ReasonCode") == "Inactive" ? ReasonCode::Inactive : ReasonCode::Unspecified,

				res->getString("Voters_Status") == "Active" ? Status::Active :
				res->getString("Voters_Status") == "ActiveMilitary" ? Status::ActiveMilitary :
				res->getString("Voters_Status") == "ActiveSpecialFederal" ? Status::ActiveSpecialFederal :
				res->getString("Voters_Status") == "ActiveSpecialPresidential" ? Status::ActiveSpecialPresidential :
				res->getString("Voters_Status") == "ActiveUOCAVA" ? Status::ActiveUOCAVA :
				res->getString("Voters_Status") == "Inactive" ? Status::Inactive :
				res->getString("Voters_Status") == "Purged" ? Status::Purged :
				res->getString("Voters_Status") == "Prereg17YearOlds" ? Status::Prereg17YearOlds :
				res->getString("Voters_Status") == "Confirmation" ? Status::Confirmation : Status::Unspecified,

				res->getInt("VotersMailingAddress_ID"),
				res->getString("Voters_IDRequired") == "yes",
				res->getString("Voters_IDMet") == "yes",

				mysqlDateToInt(res->getString("Voters_ApplyDate")),

				res->getString("Voters_RegSource") == "Agency" ? RegSource::Agency :
				res->getString("Voters_RegSource") == "CBOE" ? RegSource::CBOE :
				res->getString("Voters_RegSource") == "DMV" ? RegSource::DMV :
				res->getString("Voters_RegSource") == "LocalRegistrar" ? RegSource::LocalRegistrar :
				res->getString("Voters_RegSource") == "MailIn" ? RegSource::MailIn :
				res->getString("Voters_RegSource") == "School" ? RegSource::School : RegSource::Unspecified,

				mysqlDateToInt(res->getString("Voters_DateInactive")),
				mysqlDateToInt(res->getString("Voters_DatePurged")),
				ToUpperAccents(res->getString("Voters_CountyVoterNumber")),
				res->getString("Voters_RMBActive") == "yes"
			);

			Map[voter] = res->getInt("Voters_ID");

			if ( ++Counter % 500000 == 0 ) {
				std::cout << "Voter reached: " << Counter << std::endl;
			}
		}

		delete res;
		delete stmt;

		// Record the end time
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

		std::cout << "Query executed successfully." << std::endl;

	} catch (sql::SQLException &e) {
		std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
	}
}

// Load 
void DatabaseConnector::executeLoadVotersIdxQuery(const std::string& query, VoterIdxMap& Map) {
	
		std::cout << "DatabaseConnector::executeLoadVoterIdx" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
    	
    	int Counter = 0;
	
	
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
       
           while (res->next()) {
             VoterIdx voteridx (
                res->getInt("DataLastName_ID"),
                res->getInt("DataFirstName_ID"),
                res->getInt("DataMiddleName_ID"),
                 ToUpperAccents(res->getString("VotersIndexes_Suffix")),
                mysqlDateToInt(res->getString("VotersIndexes_DOB")),
                res->getString("VotersIndexes_UniqStateVoterID")
            );
            Map[voteridx] = res->getInt("VotersIndexes_ID");
            
            if ( ++Counter % 500000 == 0 ) {
          		std::cout << "Voter Index: " << Counter << std::endl;
          	}

        }

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

void DatabaseConnector::executeLoadVotersComplementInfoQuery(const std::string& query, VoterComplementInfoMap& Map){
	std::cout << "DatabaseConnector::executeLoadVotersComplementInfoQuery" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
    	int Counter = 0;
	
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
        

           while (res->next()) {
             VoterComplementInfo votercomplementinfo(
								res->getInt("Voters_ID"),
								 ToUpperAccents(res->getString("VotersComplementInfo_PrevName")),
								 ToUpperAccents(res->getString("VotersComplementInfo_PrevAddress")),
								res->getInt("DataCountyID_PrevCounty"),
								res->getInt("VotersComplementInfo_LastYearVoted"),
								res->getInt("VotersComplementInfo_LastDateVoted"),
								 ToUpperAccents(res->getString("VotersComplementInfo_OtherParty"))
            );
            Map[votercomplementinfo] = res->getInt("VotersComplementInfo_ID");
            
              if ( ++Counter % 500000 == 0 ) {
          		std::cout << "Voter VotersComplementInfo: " << Counter << std::endl;
          	}

        }

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

void DatabaseConnector::executeLoadDataMailingAddressQuery(const std::string& query, DataMailingAddressMap& Map) {
	std::cout << "DatabaseConnector::executeLoadDataMailingAddressQuery" << std::endl;
	std::cout << "Executing the query: " << query << std::endl;
	int Counter = 0;
	
  try {    	
		auto start = std::chrono::high_resolution_clock::now();

		sql::Statement* stmt;
		sql::ResultSet* res;

		stmt = con->createStatement();
		res = stmt->executeQuery(query);

		   while (res->next()) {
		     DataMailingAddress datamailingaddress(
		         ToUpperAccents(res->getString("DataMailingAddress_Line1")),
		         ToUpperAccents(res->getString("DataMailingAddress_Line2")),
		         ToUpperAccents(res->getString("DataMailingAddress_Line3")),
		         ToUpperAccents(res->getString("DataMailingAddress_Line4"))
		    );
		    Map[datamailingaddress] = res->getInt("DataMailingAddress_ID");

				if ( ++Counter % 500000 == 0 ) {
		  		std::cout << "Voter DataMailingAddress: " << Counter << std::endl;
		  	}

		}

		delete res;
		delete stmt;

		// Record the end time
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

		std::cout << "Query executed successfully." << std::endl;
      	
  } catch (sql::SQLException &e) {
     std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
  }
}

void DatabaseConnector::executeLoadDataDistrictQuery(const std::string& query, DataDistrictMap& Map) {
	std::cout << "DatabaseConnector::executeLoadDataDistrictQuery" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
	int Counter = 0;
	
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
        

           while (res->next()) {
             DataDistrict datadistrict(
                res->getInt("DataCounty_ID"),
                res->getInt("DataDistrict_Electoral"),
                res->getInt("DataDistrict_StateAssembly"),
                res->getInt("DataDistrict_StateSenate"),
                res->getInt("DataDistrict_Legislative"),
                 ToUpperAccents(res->getString("DataDistrict_Ward")),
                res->getInt("DataDistrict_Congress")
            );
            Map[datadistrict] = res->getInt("DataDistrict_ID");
						if ( ++Counter % 500000 == 0 ) {
								std::cout << "Voter DataDistrict: " << Counter << std::endl;
							}
						}

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

void DatabaseConnector::executeLoadDataDistrictTemporalQuery(const std::string& query, DataDistrictTemporalMap& Map) {
	std::cout << "DatabaseConnector::executeLoadDataDistrictTemporalQuery" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
	int Counter = 0;
	
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
       
           while (res->next()) {
             DataDistrictTemporal datadistricttemporal(
                res->getInt("DataDistrictCycle_ID"),
                res->getInt("DataHouse_ID"),
                res->getInt("DataDistrict_ID")
            );
            Map[datadistricttemporal] = res->getInt("DataDistrictTemporal_ID");
						if ( ++Counter % 500000 == 0 ) {
								std::cout << "Voter DataDistrictTemporal: " << Counter << std::endl;
							}
        }

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

void DatabaseConnector::executeLoadDataHouseQuery(const std::string& query, DataHouseMap& Map) {
	std::cout << "DatabaseConnector::executeLoadDataHouseQuery" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
	int Counter = 0;
	
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
        

           while (res->next()) {
             DataHouse datahouse(
                res->getInt("DataAddress_ID"),
                 ToUpperAccents(res->getString("DataHouse_Type")),
                 ToUpperAccents(res->getString("DataHouse_Apt")),
                res->getInt("DataDistrictTown_ID"),
                res->getInt("DataStreetNonStdFormat_ID"),
                res->getInt("DataHouse_BIN")
            );
            
            Map[datahouse] = res->getInt("DataHouse_ID");
						if ( ++Counter % 500000 == 0 ) {
								std::cout << "Voter DataHouse: " << Counter << std::endl;
							}
        }

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

void DatabaseConnector::executeLoadDataAddressQuery(const std::string& query, DataAddressMap& Map) {
	std::cout << "DatabaseConnector::executeLoadDataAddressQuery" << std::endl;
    std::cout << "Executing the query: " << query << std::endl;
	int Counter = 0;
    try {    	
    		auto start = std::chrono::high_resolution_clock::now();
    	
        sql::Statement* stmt;
        sql::ResultSet* res;

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
   
				while (res->next()) {
					DataAddress dataaddress(
						ToUpperAccents(res->getString("DataAddress_HouseNumber")),
						ToUpperAccents(res->getString("DataAddress_FracAddress")),
						ToUpperAccents(res->getString("DataAddress_PreStreet")),
						res->getInt("DataStreet_ID"),
						ToUpperAccents(res->getString("DataAddress_PostStreet")),
						res->getInt("DataCity_ID"),
						res->getInt("DataCounty_ID"),
						ToUpperAccents(res->getString("DataAddress_zipcode")),
						ToUpperAccents(res->getString("DataAddress_zip4")),
						res->getInt("Cordinate_ID"),
						res->getInt("PG_OSM_osmid")
					);
					
					Map[dataaddress] = res->getInt("DataAddress_ID");
					
					if ( ++Counter % 500000 == 0 ) {
						std::cout << "Voter DataAddress: " << Counter << std::endl;
					}
        }

        delete res;
        delete stmt;
        
        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();
 				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Query executed in " << duration.count() << " milliseconds." << std::endl;

        std::cout << "Query executed successfully." << std::endl;
        	
    } catch (sql::SQLException &e) {
        std::cerr << "Error: Could not execute query. " << e.what() << std::endl;
    }
}

std::string DatabaseConnector::intToMySQLDate(int dateInt) {
		if (dateInt > 0) {
	
	    int year = dateInt / 10000;
  	  int month = (dateInt / 100) % 100;
    	int day = dateInt % 100;

    	std::stringstream ss;
    	ss << std::setw(4) << std::setfill('0') << year << '-'
      	 << std::setw(2) << std::setfill('0') << month << '-'
       	<< std::setw(2) << std::setfill('0') << day;
    	return ss.str();
    }
    return "";
}

int DatabaseConnector::mysqlDateToInt(const std::string& mysqlDate) {
    if (mysqlDate.size() != 10 || mysqlDate[4] != '-' || mysqlDate[7] != '-') {
        // std::cerr << "Invalid MySQL date format." << std::endl;
        return 0;  // or throw an exception, or handle it in some other appropriate way
    }
    
    int year, month, day;
    sscanf(mysqlDate.c_str(), "%d-%d-%d", &year, &month, &day);
    return year * 10000 + month * 100 + day;
}

std::string DatabaseConnector::ToUpperAccents(const std::string& input) {
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
	  		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:  	// àáâãäå
				case 0xA8: case 0xA9: case 0xAA: case 0xAB:  												// èéêë
				case 0xAC: case 0xAD: case 0xAE: case 0xAF: 												// ìíîï
				case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB8: 	// òóôõöø
				case 0xB9: case 0xBA: case 0xBB: case 0xBC: 												// ùúûü
				case 0xA7: case 0xB1: 																							// çñ
					result += (c - 32);
          break;
				
				default: case 0x9F: 	// ß
      		result += c;
      		break;
			}
		}
	}
	return result;
}