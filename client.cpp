#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
// cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// json
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;
const string CONFIG_PATH = "config.json";
const int BUFFER_SIZE = 1024;


//read from .json file & convert it to json obj 
json readJsonFile(string filePath) {
  ifstream f(filePath);
  json data = json::parse(f);
  f.close();
  return data;
}

class Client {
    int port;
    int socketFd;
    char buffer[BUFFER_SIZE];

    string hostName = "127.0.0.1";
    int connectServer() {
      int fd;
      struct sockaddr_in server_address;
      fd = socket(AF_INET, SOCK_STREAM, 0);

      server_address.sin_family = AF_INET;
      server_address.sin_port = htons(port);
      server_address.sin_addr.s_addr = inet_addr(hostName.c_str());

      if (connect(fd, (struct sockaddr *)&server_address,
       sizeof(server_address)) < 0) {   // checking for errors
          printf("Error in connecting to server\n");
          return -1;
      }
      return fd;
    }
  public:
    Client(string configFile) {
      // read config file
      json config = readJsonFile(configFile);
      // get port and host
      port = config["commandChannelPort"];
      hostName = config["hostName"];
    }
    void run() {
      write(1, "welcome to hotel transilvania\n", 31);
      socketFd = connectServer();
      if (socketFd < 0 ) {
        perror("server tcp socket failed");
        exit(EXIT_FAILURE);
      }
      string msg;
      while(true) {
        memset(buffer, 0, 1024);
        cout << "please enter your msg\n" << endl;
        cin >> msg;
        send(socketFd, msg.c_str(), msg.length()+1, 0);
        msg.clear();
        recv(socketFd, buffer, BUFFER_SIZE, 0);
        msg = string(buffer);
        cout << msg << "\n above is the recieved masssage\n" << endl;
      }
    }



};

int main() {
  Client client = Client(CONFIG_PATH);
  client.run();

}