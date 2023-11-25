#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H

#include "Voter.h"
#include <string>
#include <cppconn/connection.h>
#include <vector>
#include <iostream>
#include <exception>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <chrono>

// Database connection constants
const std::string  DB_HOST = "192.168.199.18";
const unsigned int DB_PORT = 3306;
const std::string  DB_USER = "usracct";
const std::string  DB_PASS = "usracct";
const std::string  DB_NAME = "RepMyBlock";
  
class DatabaseConnector {
  public:
    DatabaseConnector();
    ~DatabaseConnector();
    // bool connect();
    sql::Connection* getConnection(void);
    std::string CustomEscapeString(const std::string&);
    sql::ResultSet* executeQuery(const std::string&);
    void executeInsert(const std::string&);
    void deleteResource(sql::ResultSet*);
      
  private:
    sql::Connection* con;
    sql::mysql::MySQL_Driver* driver;
    sql::Connection *ConnectToDB(void);
    void DisconectFromDB(void);
      
};

#endif //DATABASECONNECTOR_H