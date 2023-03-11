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

struct Client{
int socket_fd;
bool isAdmin;
int index; // save client id if sth could happen to array. 
};

class Server {
    private:
        int port;
        string host_name;
        int max_clients;
        int serverFd;
        vector<Admin> admins;
        vector<User> users;
        vector<Room> rooms;
        char buffer[BUFFER_SIZE];

  // accept a connection
    int acceptClient(int server_fd=-2);
  
  // setting up a server
    int setupServerSocket();

      

  public:
  void run();
  // constructor
  Server(string configPath) {
    // read config file
    json config = readJsonFile(configPath);
    // get portNumber
    port = config["commandChannelPort"];
    host_name = config["hostName"];
    max_clients = config["maxClients"];
  }
  
};

