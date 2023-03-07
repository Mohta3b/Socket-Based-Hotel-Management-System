#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include "Log.hpp"

#define SYSTEM 0
#define USER 1
#define ADMIN 2

void checkDirectory(int type, int user_id)
{
    const char *server_log_directory = "../Server/Log";
    struct stat sb;
    if (!(stat(server_log_directory, &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(server_log_directory);

    const char *client_log_directory = "../Client/Log";
    if (!(stat(client_log_directory, &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(client_log_directory);

    const char *user_log_directory = "../Client/Log/Users";
    if (!(stat(user_log_directory, &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(user_log_directory);

    const char *admin_log_directory = "../Client/Log/Admins";
    if (!(stat(admin_log_directory, &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(admin_log_directory);
    
    std::string path;
    if (type == USER)
        path = "../Client/Logs/Users/"+std::to_string(user_id);
    else if (type == ADMIN)
        path = "../Client/Logs/Admins/"+std::to_string(user_id);

    if (!(stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(path);
}

void logEvent(int type, int log_id, std::string detail = "", int user_id = 0)
{
    std::string log;
    // write code to append or create txt file and save current time, logs[log_id] and detail if available in current directory
    time_t now = time(0);
    char *date_time = ctime(&now);
    log += date_time + '\n';

    checkDirectory(type, user_id);

    if (type == SYSTEM) // system type
    {
        log += "type: SYSTEM\n";
        log += "description: " + logs[log_id] + '\n';
        log += "detail: " + detail + '\n';
    }
    else if (type == USER) // user type
    {
        log += "type: USER (id: " + std::to_string(user_id) + ")\n";
        log += "description: " + logs[log_id] + '\n';
        log += "detail: " + detail + '\n';

        std::ofstream userLogFile;
        std::string path = "../Client/Logs/Users/"+ std::to_string(user_id) + "/log.txt";
        userLogFile.open(path, std::ios::app);
        userLogFile << log << std::endl;
        userLogFile.close();
    }
    else if (type == ADMIN) // admin type
    {
        log += "type: ADMIN (id: " + std::to_string(user_id) + ")\n";
        log += "description: " + logs[log_id] + '\n';
        log += "detail: " + detail + '\n';

        std::ofstream adminLogFile;
        std::string path = "../Client/Logs/Admins/"+ std::to_string(user_id) + "/log.txt";
        adminLogFile.open(path, std::ios::app);
        adminLogFile << log << std::endl;
        adminLogFile.close();
    }
    else
    {
        log += "type: UNKNOWN\n";
        log += "description: " + logs[log_id] + '\n';
        log += "detail: " + detail + '\n';
    }

    std::ofstream serverLogFile;

    serverLogFile.open("../Server/Log/log.txt", std::ios::app);

    serverLogFile << log << std::endl;

    serverLogFile.close();
}