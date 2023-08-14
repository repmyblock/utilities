#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <ctime>
#include <string>
#include <map>

const std::string DB_HOST = "data.theochino.us";
const unsigned int DB_PORT = 3306;
const std::string DB_USER = "usracct";
const std::string DB_PASS = "usracct";
const std::string DB_NAME = "RepMyBlockTwo";

int main() {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;

    driver = sql::mysql::get_mysql_driver_instance();
    conn = driver->connect(DB_HOST, DB_USER, DB_PASS);
    conn->setSchema(DB_NAME);

    std::clock_t start_time = std::clock();
    std::map<std::string, int> data_map;

    std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT DataFirstName_ID, DataFirstName_Text FROM DataFirstName"));
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

    while (res->next()) {
        int index = res->getInt("DataFirstName_ID");
        std::string name = res->getString("DataFirstName_Text");
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        data_map[name] = index;
    }

    double loading_time = (std::clock() - start_time) / (double)CLOCKS_PER_SEC;
    std::cout << "C++ loading the DB Information in " << loading_time << " seconds" << std::endl;

    std::cout << "What is the name you are seeking?" << std::endl;
    std::string name;
    std::getline(std::cin, name);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    start_time = std::clock();
    int found_index = data_map.find(name) != data_map.end() ? data_map[name] : -1;
    
    if (found_index != -1) {
        std::cout << "The name " << name << " is index " << found_index << std::endl;
    } else {
        std::cout << "Name not found" << std::endl;
    }

    double search_time = (std::clock() - start_time) / (double)CLOCKS_PER_SEC;
    std::cout << "Finding the Index Information in " << search_time << " seconds" << std::endl;

    delete conn;
    return 0;
}
