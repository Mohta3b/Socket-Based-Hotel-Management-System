#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "../Client/admin.hpp"
#include "../Client/room.hpp"
#include "../Client/user.hpp"
#include "utility.hpp"

using json = nlohmann::json;
using namespace std;
const int BUFFER_SIZE = 1024;
const string CONFIG_PATH = "config.json";
const int NOTREGISTERED = -1;
struct Client
{
  Client() {
    socket_fd = NOTREGISTERED;
    isAdmin = false;
    index = NOTREGISTERED;
    command = "";
    argsNum = 0;
    commandID = -1;
  }
  int socket_fd;
  bool isAdmin;
  int index; // save client id if sth could happen to array.
  string command;
  int argsNum;
  int commandID;
};

class Server
{
private:
  int port;
  string date;
  string host_name;
  int max_clients;
  int serverFd;
  vector<Admin> admins;
  vector<User> users;
  vector<Room> rooms;
  vector<Client> onlineClients;
  char buffer[BUFFER_SIZE];

  // accept a connection
  int acceptClient(int server_fd = -2);
  // setting up a server
  int setupServerSocket();
  // set initial time
  void Server::setDate();
  //  find client
  Client& findClient(int socket_fd);
  void readRoomsUserFiles(string roomsPath, string usersPath);
  //  is repeated
  bool isRepeatedName(string name);
  // commands
  // client
  void signUp();
  void login();
  void viewUserInfo();
  void viewRoomInfo();
  void editInfo();
  void logout();
  // user
  void bookRoom();
  void cancelReserve();
  void leaveRoom();
  // admin 
  void viewAllUsersInfo();
  void passDay();
  void banGuest();
  void addRoom();
  void modifyRoom();
  void deleteRoom();
public:
  void run();
  // constructor
  Server(string configPath=CONFIG_PATH)
  {
    // read config file
    json config = readJsonFile(configPath);
    // get portNumber
    port = config["commandChannelPort"];
    host_name = config["hostName"];
    max_clients = config["maxClients"];
  }
};
 