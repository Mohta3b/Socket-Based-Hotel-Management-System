#include "server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <arpa/inet.h>
#include "../Server/Log.hpp"

const int BUFFER_SIZE = 1024;


int Server::acceptClient(int server_fd=-2) { // -2 STANDS FOR EMPTY
      if(server_fd == -2)
        server_fd = serverFd;
      int client_fd;
      struct sockaddr_in client_address;
      int address_len = sizeof(client_address);
      client_fd = accept(server_fd, (struct sockaddr *)&client_address,
      (socklen_t *)&address_len);
      return client_fd;
  } 

  int Server::setupServerSocket() {
      sockaddr_in address;
      int server_fd;
      server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (server_fd < 0) {
        perror("server tcp socket failed");
        exit(EXIT_FAILURE);
      }
      int opt = 1;
      setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

      address.sin_family = AF_INET;
      address.sin_addr.s_addr = inet_addr(host_name.c_str());;
      address.sin_port = htons(port);

      if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
      {
          perror("bind failed\n");
          printf("error in binding\n");
          return -1;
      }

      listen(server_fd, this->max_clients);
      
      return server_fd;
  }

  void Server::run() {
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_ZERO(&working_set);
    int byteCount;
    // setup server
    this->serverFd = setupServerSocket();
    if(serverFd < 0) 
    {
      logEvent(SYSTEM,22,"Error in tcp setup!");
      exit(-1);
    }
    string msg = "Server is running\n";
    logEvent(SYSTEM,0,msg);
    // send(1, msg.c_str(), msg.length(), 0);
    // write(1, msg.c_str(), msg.length());
    //printf("before while on line 111\n\n");

    while(true) {
        FD_SET(0, &master_set);
        FD_SET(serverFd,&master_set);
        //cout << "new round && serverFd is: "<<serverFd << endl;
        working_set = master_set;
        //printf(">>");
        fflush(stdout);
        if (select(FD_SETSIZE, &working_set, NULL, NULL, NULL) < 0) {
            continue;
        }
        for (int i = 0; i < FD_SETSIZE; i++) {
            memset(buffer, 0, 1024);
            if (FD_ISSET(i, &working_set)) {
                printf("\nfd: %d\n", i);
                if (i == 0) { // stdin 
                    read(0, buffer, BUFFER_SIZE);
                    buffer[strlen(buffer) - 1] = '\0';
                }
                else if(i == serverFd) { // clients(user or admin)
                  int newClient = acceptClient();
                  if(newClient < 0) {
                    cout << "error on accepting new client\n" << endl;
                  }
                  cout << "new client: " << newClient << endl; 
                  FD_SET(newClient, &master_set);
                }
                else {
                  msg.clear();
                  
                  if((byteCount = recv(i, buffer, BUFFER_SIZE, 0)) == -1) {
                    cout << i << "is hooooshdoooded no to rcv from" << endl;
                    FD_CLR(i, &master_set);
                    continue;
                  }
                  //  if there is on one then recv return 0 i guess so we 
                  // should consider this.
                  
                  msg= string(buffer);
                  msg += " server is very happy tp meet you\n";
                  cout << msg << "\t is to send to clinet\n" << endl;
                  if((byteCount = send(i, msg.c_str(), msg.length()+1,0)) == -1) {
                    cout << i << " is hooooshdoooded no one to send to" << endl;
                    FD_CLR(i, &master_set);
                    continue;
                  }
                  
                }
            }
        }   
    }
  }