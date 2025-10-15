#include "DatabaseConnector.h"
#include "Voter.h"

#include <cppconn/prepared_statement.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <cstdlib> // For getenv()
#include <exception>
#include <chrono>

#include <iostream>
#include <string>
#include <exception>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <thread>
#include <chrono>

DatabaseConnector::DatabaseConnector(const std::string& FileTableDate) : con(nullptr) {
	TableDate = FileTableDate;
  ConnectToDB();
}

DatabaseConnector::~DatabaseConnector() {}
	
std::string DatabaseConnector::ReturnMysqlFileDate(void) {
	return TableDate;
}


bool DatabaseConnector::ConnectToDB(void) {
	// Check if the connection is already established
	
	/*	
	if (con != nullptr) {
		std::cout << HI_YELLOW << "NULLPTR Con != " << NC << std::endl;
		std::cout << HI_YELLOW << "DOING VALIDITY CHECK" << NC << std::endl;

		/*************************** 
		 *** THIS IS THE PROBLEM ***
		 ***************************			
		 
		if ( con->isValid() ) {
			std::cout << "isValid -> YES " << HI_RED <<  NC<< std::endl;
		} else {
			std::cout << "idValud -> NO " << HI_RED <<  NC << std::endl;
		}
			
	} else {
		std::cout << "NULLPTR Con else " << std::endl;	
	}
	*/
	
	if (con != nullptr ) {
		if ( con->isValid() ) {
			//std::cout << HI_BLUE << "NULL STRING ConnectToDB ... " << NC << std::endl;
	  	return true;
		}
	}/* else {
		std::cout << "Else isValid ... " << std::endl;
	}

	std::cout << "Load DB Config: " << con << std::endl;
*/
	auto dbConfig = loadDbConfig();

	try {
	  sql::mysql::MySQL_Driver* driver;
	  driver = sql::mysql::get_mysql_driver_instance();

	  std::string dbHost = dbConfig["DB_HOST"];
	  unsigned int dbPort = std::stoi(dbConfig["DB_PORT"]);
	  std::string dbUser = dbConfig["DB_USER"];
	  std::string dbPass = dbConfig["DB_PASS"];
	  std::string dbName = dbConfig["DB_NAME"];

	  con = driver->connect(dbHost + ":" + std::to_string(dbPort), dbUser, dbPass);
	  con->setSchema(dbName);
	  con->setClientOption("characterSetResults", "utf8mb4");

	  std::unique_ptr<sql::PreparedStatement> pstmt;
	  pstmt.reset(con->prepareStatement("SET NAMES utf8mb4"));
	  pstmt->execute();

		//std::cout << HI_YELLOW << "Done with connecting to the database" << NC << std::endl;

	  return true;
	  
	} catch (sql::SQLException &e) {
	  std::cerr << std::endl << "DatabaseConnector::ConnectToDB Error: Could not connect to database." << std::endl << e.what() << std::endl;
	  return false;
	}
}

void DatabaseConnector::DisconnectFromDB(void) {
	
  if(con != nullptr) {
    delete con;
 		con = nullptr;   
  }

}

sql::Connection* DatabaseConnector::getConnection() {
  return con;
}

sql::ResultSet* DatabaseConnector::executeQuery(const std::string& sql) {
	int maxRetries = 3; // Maximum number of retry attempts
	sql::ResultSet* res = nullptr;
	sql::Statement* stmt = nullptr;
		
	//std::cout << HI_GREEN << "Starting to Execute Query with SQL: " << NC << HI_WHITE << sql << NC << std::endl;

	for (int attempt = 0; attempt < maxRetries; ++attempt) {
		
		//std::cout << HI_YELLOW << "Inside Execute Query: Attempt: " << NC << HI_RED << attempt << NC << std::endl;
		if (! ConnectToDB() ) { // Ensure the connection is active, attempt to reconnect if not
		  std::cerr << "Failed to establish database connection. Retrying in 5 seconds." << std::endl;
		  std::this_thread::sleep_for(std::chrono::seconds(5));
		  continue;
		}

		try {

			//std::cout << "Create the statement" << std::endl;
		  stmt = con->createStatement();

			//std::cout << "Execute the query"  << std::endl;
		  res = stmt->executeQuery(sql);
		  
 			//std::cout << "Done Executing the query statement" << std::endl;

		  break; // Break the loop if query is successful
		} catch (sql::SQLException &e) {
		  std::cerr << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
		  if (stmt != nullptr) {
	      delete stmt;
	      stmt = nullptr;
		  }
		  DisconnectFromDB();
		  exit(1);
		  
		} catch (std::exception &e) {
		  std::cerr << "Error: " << e.what() << std::endl;
			if (stmt != nullptr) {
			  delete stmt;
			  stmt = nullptr;
			}
			DisconnectFromDB();
			exit(1);
		}
		
		// std::cout << "Done with the catchesed" << std::endl;
	}

	if (stmt != nullptr) {
		delete stmt;
	}
	
	// std::cout << "Disconnecting" << std::endl;
	// DisconnectFromDB();
//std::cout << "Disconnected Inside Execute Query and returning res: " << HI_YELLOW << res << NC << std::endl;
	
	return res;
}

/*
sql::ResultSet* DatabaseConnector::executeQuery(const std::string& sql) {
 
	sql::Statement* stmt = con->createStatement(); 
	sql::ResultSet* res;
  try {	
  	res = stmt->executeQuery(sql);
	}
	catch (sql::SQLException &e) {
  	std::cerr << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before retrying
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
	 	 	
	delete stmt;
  DisconectFromDB();
  return res;
}
*/ 

void DatabaseConnector::executeInsert(const std::string& sql) {
    int maxRetries = 3; // Maximum number of retry attempts
    sql::Statement* stmt = nullptr;

    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        if (!ConnectToDB()) { // Ensure the connection is active, attempt to reconnect if not
            std::cerr << "Failed to establish database connection. Retrying in 5 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        try {
            stmt = con->createStatement();
            stmt->executeUpdate(sql);
            break; // Break the loop if insert is successful
        } catch (sql::SQLException &e) {
            std::cerr << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
            if (stmt != nullptr) {
                delete stmt;
                stmt = nullptr;
            }
            DisconnectFromDB();
            if (attempt < maxRetries - 1) {
                std::cerr << "SQL Error: Attempting again " << attempt + 1 << " of " << maxRetries << " tries." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before retrying
            }
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            if (stmt != nullptr) {
                delete stmt;
                stmt = nullptr;
            }
            DisconnectFromDB();
            break; // Break the loop if it's a non-SQL exception
        }
    }

    if (stmt != nullptr) {
        delete stmt;
    }
    DisconnectFromDB();
}


/*
void DatabaseConnector::executeInsert(const std::string& sql) {
  ConnectToDB();
  sql::Statement* stmt = con->createStatement(); 
  stmt->executeUpdate(sql);
  delete stmt;
  DisconnectFromDB();
}
*/

void DatabaseConnector::deleteResource(sql::ResultSet* res) {
  if (res != nullptr) {
    delete res;
  }
}

std::string DatabaseConnector::CustomEscapeString(const std::string& input) {
  std::string escapedString;
  for (char c : input) {
    switch (c) {
      case '\'': escapedString += "\\'"; break;
      case '\"': escapedString += "\\\""; break;
      case '\\': escapedString += "\\\\"; break;
      case '\n': escapedString += "\\n"; break;
      case '\r': escapedString += "\\r"; break;
      case '\t': escapedString += "\\t"; break;
      default: escapedString += c;
    }
  }
  return escapedString;
}

std::map<std::string, std::string> DatabaseConnector::loadDbConfig(void) {
  std::map<std::string, std::string> dbConfig;
  const char* homeDir = getenv("HOME");
  if (homeDir == nullptr) {
    std::cerr << "Error: HOME environment variable not set." << std::endl;
    exit(1);
  }

  std::string configFile = std::string(homeDir) + "/.dbprocess";
  std::ifstream infile(configFile);

  if (!infile.is_open()) {
    std::cerr << "Error: Unable to open config file: " << configFile << std::endl;
    exit(1);
  }

  std::string line;
  while (std::getline(infile, line)) {
    size_t delimiterPos = line.find(":");
    if (delimiterPos != std::string::npos) {
      std::string key = trim(line.substr(0, delimiterPos));
      std::string value = trim(line.substr(delimiterPos + 1));
      dbConfig[key] = value;
    }
  }

  return dbConfig;
}

std::string DatabaseConnector::trim(const std::string& str) {
	auto start = str.begin();
	while (start != str.end() && std::isspace(*start)) {
	  start++;
	}

	auto end = str.end();
	do {
	  end--;
	} while (std::distance(start, end) > 0 && std::isspace(*end));

	return std::string(start, end + 1);
}