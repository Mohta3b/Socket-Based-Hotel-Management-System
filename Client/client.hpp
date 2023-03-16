#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
// cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
// json
#include "../Server/utility.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;
const std::string CONFIG_PATH = "../Server/config.json";
const int BUFFER_SIZE = 4096;

map<string,string> commandsHelp = {
  {"signup","<password>\n<balance>\n<phone>\n<address>"},
  {"login","<username> <password>"},
  {"book", "<room_num> <num_of_beds> <check_in_date> <check_out_date>"},
  {"cancel", "<RoomNum> <number of beds>"},
  {"passDay", "<value>"},
  {"edit_Info_user", "<new password>\n<phone>\n<address>"},
  {"edit_Info_admin", "<new password>"},
  {"leaving_room", "<value>"},
  {"add_room", "<roomNum> <max capacity> <price>"},
  {"modify_room", "<roomNum> <new max capacity> <new price>"},
  {"delete_room", "remove <RoomNum>"},
  {"ban_guest", "<roomNum>"}
};
// json readJsonFile(string filePath) {
//   ifstream f(filePath);
//   json data = json::parse(f);
//   f.close();
//   return data;
// }
enum ClientStatus {
  NOT_LOGIN=-1,
  USER,
  ADMIN
};
class Client {
    int port;
    int socketFd;
    ClientStatus status;
    char buffer[BUFFER_SIZE];
    int argCount;
    int commandId;
    string hostName = "127.0.0.1";

    int connectServer();
    // show menu
    // 
  public:
    void showCommandHelp(int commandId);
    string RecieveFromServer();
    void showMenu();
    bool getCommand(string Help, int errorCode);
    void getCancelCommand(string Help, int errorCode);
    Client(string configFile) {
      // read config file
      json config = readJsonFile(configFile);
      // get port and host
      port = config["commandChannelPort"];
      hostName = config["hostName"];
      status = NOT_LOGIN;
    }

    void run();
};
