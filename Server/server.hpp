#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <regex>
#include "../Client/admin.hpp"
#include "../Client/room.hpp"
#include "../Client/user.hpp"
#include "utility.hpp"

using json = nlohmann::json;
using namespace std;
const int BUFFER_SIZE = 1024;
const string CONFIG_PATH = "config.json";
const int NOTREGISTERED = -1;
enum Status {PENDDING, ACCEPTED, REJECTED};

const string date_pattern = "(^(((0[1-9]|1[0-9]|2[0-8])\
[-](0[1-9]|1[012]))|((29|30|31)\
[-](0[13578]|1[02]))|((29|30)\
[-](0[4,6,9]|11)))\
[-](19|[2-9][0-9])\\d\\d$)|(^29[-]02\
[-](19|[2-9][0-9])(00|04|08|12|16|20|24|28|32|36|40|44|48|52|56|60|64|68|72|76|80|84|88|92|96)$)";
const regex dateRegex(date_pattern);

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
  Client(int socket_fd, int index){
    this->socket_fd = socket_fd;
    this->index = index;
    isAdmin = false;
    command = "";
    argsNum = 0;
    commandID = NOTREGISTERED;
  }

  int socket_fd;
  bool isAdmin;
  int index; // save client id if sth could happen to array.
  string command;
  int argsNum;
  int commandID;
};
struct MiniClient {
  string password;
  int index;
  bool isAdmin;
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
  // uttilities
  //  find client
  Client& findClient(int socket_fd);
  void readRoomsUserFiles(string roomsPath, string usersPath);
  //  is repeated
  bool isRepeatedName(string name);
  // find unique user id
  int findUniqueID();
  // add new user
  void Server::addUser(string args);
  // find user or admin by name
  MiniClient findUserAdminByName(string name);
  // commands
  // client
  Status signUp(Client& client, string& msg);
  void login(Client& client, string& msg);
  void logout(Client& client, string& msg);
  void viewUserInfo(Client& client, string& msg);
  void viewRoomsInfo(Client& client, string& msg, std::vector<string>& command);
  void editInfo(Client& client, string& msg, vector<string>& command);
  // user
  void bookRoom();
  void cancelReserve();
  void leaveRoom();
  // admin 
  void viewAllUsersInfo(Client& client, string& msg);
  void passDay();
  void banGuest();
  void addRoom();
  void modifyRoom();
  void deleteRoom();
  void bookRoom(Client& client, string& msg,vector<string>& commands);
  void cancelReserve(Client& client, string& msg,vector<string>& commands);
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
 