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

class DatabaseConnector {
  public:
    DatabaseConnector(const std::string&);
    ~DatabaseConnector();
    // bool connect();
    sql::Connection* getConnection(void);
    std::string CustomEscapeString(const std::string&);
    sql::ResultSet* executeQuery(const std::string&);
    void executeInsert(const std::string&);
    void deleteResource(sql::ResultSet*);
    std::string ReturnMysqlFileDate(void);
      
  private:
    sql::Connection* con;
    sql::mysql::MySQL_Driver* driver;
    sql::Connection *ConnectToDB(void);
    void DisconectFromDB(void);
    std::map<std::string, std::string>loadDbConfig(void);   
    std::string trim(const std::string&);
    std::string TableDate;
};

#endif //DATABASECONNECTOR_H