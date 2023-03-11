#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;
const string CONFIG_PATH = "config.json";
const int BUFFER_SIZE = 1024;

/*
struct UDP_Config {
    int sock;
    struct sockaddr_in bc_address;
};

UDP_Config setupUDPserver(int port)
{
    int sock, broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;
    TCP_Config result;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    result.bc_address = bc_address;
    if (bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address)) < 0)
    {
        sock = -1;
    }
    result.sock = sock;
    return result;
}
*/

//read from .json file & convert it to json obj 




// use select to handle multiple requests

int main() {
  Server server = Server(CONFIG_PATH);
  server.run();
  cout << "dfdgf\n";
  return 0;
}