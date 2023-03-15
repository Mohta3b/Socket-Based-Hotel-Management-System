#pragma once
#include "./client.hpp"


int Client::connectServer() {
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

void Client::run() {

    cout << "welcome to hotel transilvania\n";
    socketFd = connectServer();
    if (socketFd < 0 ) {
      perror("server tcp socket failed");
      exit(EXIT_FAILURE);
    }
    string msg;
    while(true) {
      showMenu();
      // get coomandId
      string commandMsg;
      int commandId;
      string input_msg;
      getline(cin, input_msg);
      vector<string> args = tokenize(input_msg, ' ');
      if(args.size() == 0) {
        // clear terminal screen
        system("clear");
        continue;
      }
      if(this->status == ClientStatus::NOT_LOGIN){
        // signup/login
        if(args.size() == 1 && this->argCount !=4) {
          // command is valid;
          switch (this->argCount)
          {
          case 0: // username

            break;
          case 1: // password

            break;
          case 2: // balance
            break;
          case 3: // phone
            break;
          case 4: // address

            break;
          default:
            break;
          }
        }
      } else {

      }
      // memset(buffer, 0, 1024);
      // cout << "please enter your msg\n" << endl;
      // cin >> msg;
      // send(socketFd, msg.c_str(), msg.length()+1, 0);
      // msg.clear();
      // recv(socketFd, buffer, BUFFER_SIZE, 0);
      // msg = string(buffer);
      // cout << msg << "\n above is the recieved masssage\n" << endl;
    }
}
// show menu
void Client::showMenu() {
  if(this->status == ClientStatus::NOT_LOGIN){
    cout << "please enter your choice\n" << endl;
    cout << "0. Sign up" << endl;
    cout << "1. Login\n" << endl; 
  } else if(this->status == ClientStatus::USER) {
    cout << "please enter your choice\n" << endl;
  } else if(this->status == ClientStatus::ADMIN) {
    cout << "please enter your choice\n" << endl;
  }
}
void Client::showCommandHelp(int commandId) {
  if(this->status == ClientStatus::NOT_LOGIN){
    switch (commandId) {
      case 0:
        // signup
        cout << commandsHelp.at("signup") << endl;
        break;
      case 1:
        // login
        cout << commandsHelp.at("login") << endl;
        break;
      default:
        break;
    }
  } else {
    // loggedin commands
    if(this->status == ClientStatus::ADMIN) {
      // admin commands
    } else {
      // user commands
    }
  }
}




int main() {
  Client client = Client(CONFIG_PATH);
  client.run();
  // client.showMenu();
}