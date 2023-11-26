#include "DatabaseConnector.h"
#include "Voter.h"

#include <cppconn/prepared_statement.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <cstdlib> // For getenv()

DatabaseConnector::DatabaseConnector(const std::string& FileTableDate) : con(nullptr) {
  TableDate = FileTableDate;
  ConnectToDB();
}

DatabaseConnector::~DatabaseConnector() {}
  
std::string DatabaseConnector::ReturnMysqlFileDate(void) {
  return TableDate;
}

sql::Connection *DatabaseConnector::ConnectToDB(void) {
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

  } catch (sql::SQLException &e) {
    std::cerr << std::endl << "DatabaseConnector::connect Error: Could not connect to database." << std::endl << e.what() << std::endl;
    exit(1);
  }

  return con;
}

void DatabaseConnector::DisconectFromDB(void) {
  if(con != nullptr) {
    delete con;
  }
}

sql::Connection* DatabaseConnector::getConnection() {
  return con;
}

sql::ResultSet* DatabaseConnector::executeQuery(const std::string& sql) {
  ConnectToDB();
  sql::Statement* stmt = con->createStatement(); 
  sql::ResultSet* res = stmt->executeQuery(sql);
  delete stmt;
  DisconectFromDB();
  return res;
}

void DatabaseConnector::executeInsert(const std::string& sql) {
  ConnectToDB();
  sql::Statement* stmt = con->createStatement(); 
  stmt->executeUpdate(sql);
  delete stmt;
  DisconectFromDB();
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