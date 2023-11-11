#include "DatabaseConnector.h"
#include "Voter.h"

#include <cppconn/prepared_statement.h>

DatabaseConnector::DatabaseConnector() : con(nullptr) { 
 try {
  sql::mysql::MySQL_Driver* driver;
  driver = sql::mysql::get_mysql_driver_instance();
  con = driver->connect(DB_HOST + ":" + std::to_string(DB_PORT), DB_USER, DB_PASS);
  con->setSchema(DB_NAME);
  con->setClientOption("characterSetResults", "utf8mb4");
  
  std::unique_ptr<sql::PreparedStatement> pstmt;
  pstmt.reset(con->prepareStatement("SET NAMES utf8mb4"));
  pstmt->execute();
  
  } catch (sql::SQLException &e) {
    std::cerr << std::endl << HI_RED << "DatabaseConnector::connect Error: Could not connect to database." << std::endl << e.what() << NC << std::endl;
    exit(1);
  }
}
  
DatabaseConnector::~DatabaseConnector() {
  if(con != nullptr) {
    delete con;
  }
}

sql::Connection* DatabaseConnector::getConnection() {
  return con;
}

sql::ResultSet* DatabaseConnector::executeQuery(const std::string& sql) {
  sql::Statement* stmt = con->createStatement(); 
  sql::ResultSet* res = stmt->executeQuery(sql);
  delete stmt;
  return res;
}

void DatabaseConnector::executeInsert(const std::string& sql) {
  sql::Statement* stmt = con->createStatement(); 
  stmt->executeUpdate(sql);
  delete stmt;
}

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