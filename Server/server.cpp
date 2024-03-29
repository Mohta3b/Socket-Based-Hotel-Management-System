
#include "server.hpp"


int Server::acceptClient(int server_fd)
{ // -2 STANDS FOR EMPTY
  if (server_fd == -2)
    server_fd = serverFd;
  int client_fd;
  struct sockaddr_in client_address;
  int address_len = sizeof(client_address);
  client_fd = accept(server_fd, (struct sockaddr *)&client_address,
                     (socklen_t *)&address_len);
  return client_fd;
}

int Server::setupServerSocket()
{
  sockaddr_in address;
  int server_fd;
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    perror("server tcp socket failed");
    exit(EXIT_FAILURE);
  }
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(host_name.c_str());
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

void Server::readRoomsUserFiles()
{
  json rooms = readJsonFile(this->roomsPath);
  json clients = readJsonFile(this->usersPath);
  this->admins = clients["admins"];
  cout << "admins: " << this->admins.size() << endl;
  this->users = clients["users"];
  cout << "users: " << this->users.size() << endl;
  this->rooms = rooms["rooms"];
  cout << "rooms: " << this->rooms.size() << endl;
  // sleep(3);
  // add reserved bookedclients to user
  for (auto& user: this->users) {
    for (auto &room : this->rooms)
    {
      for (auto &client : room.getBookedClients())
      {
        
        if (client->getId() == user.getId())
        {
          user.addReservedRoom(client);
        }
      }
    }
  }
}

void Server::setDate(string defaultDate) {
  // clear terminal screen
  system("clear");
  cout << "please enter server start date" << endl;
  cout << "setTime <Date Time>\nsetTime ";
  string TempDate;
  if(defaultDate != "01-01-2020") {
    cin >> TempDate;
  } else {
    TempDate = defaultDate;
  }
  // check if date is valid with regex dd-mm-yyyy
  if (!regex_match(TempDate, dateRegex)) {
    cout << "invalid date, please try again" << endl;
    logEvent(SYSTEM, 23, get_error(401));
  } else {
    this->date = TempDate;
    logEvent(SYSTEM, 0, "Set Date was successfull");
    return;
  }
  setDate("01-01-2020");
}

void Server::run()
{
  fd_set master_set, working_set;
  FD_ZERO(&master_set);
  FD_ZERO(&working_set);
  int byteCount;
  // setup server
  this->serverFd = setupServerSocket();
  // cout << "serverFd is: " << serverFd << endl;
  // sleep(5);
  if (serverFd < 0)
  {
    logEvent(SYSTEM, 22, "Error in tcp setup!");
    exit(-1);
  }
  string msg = "Server is running\t";
  logEvent(SYSTEM, 0, msg);
  setDate("01-01-2020");

  // send(1, msg.c_str(), msg.length(), 0);
  // write(1, msg.c_str(), msg.length());
  // printf("before while on line 111\n\n");
  // read json files
  readRoomsUserFiles();
  //run server
  while (true)
  {
    FD_SET(0, &master_set);
    FD_SET(serverFd, &master_set);
    // cout << "new round && serverFd is: "<<serverFd << endl;
    working_set = master_set;
    // printf(">>");
    fflush(stdout);
    if (select(FD_SETSIZE, &working_set, NULL, NULL, NULL) < 0)
    {
      continue;
    }
    for (int i = 0; i < FD_SETSIZE; i++)
    {
      memset(buffer, 0, 1024);
      if (FD_ISSET(i, &working_set))
      {
        printf("\nfd: %d\n", i);
        if (i == 0)
        { // stdin
          read(0, buffer, BUFFER_SIZE);
          buffer[strlen(buffer) - 1] = '\0';
          msg = string(buffer);
          if(msg == "exit") {
            logEvent(SYSTEM, 0, "Server is shutting down");
            // wrtie to json files
            writeJsonFile();
            // send msg to all sockets
            for (int j = 0; j < FD_SETSIZE; j++)
            {
              if (FD_ISSET(j, &master_set))
              {
                send(j, "Server is shutting down", 23, 0);
              }
            }
            exit(0);
          }
        }
        else if (i == serverFd)
        { // clients(user or admin), new clientSocket wants to connect,

          int newClient = acceptClient();
          if (newClient < 0)
          {
            cout << "error on accepting new client\n"
                 << endl;
          }
          msg = to_string(newClient) + " is connected to server";
          logEvent(SYSTEM, 2, msg);
          FD_SET(newClient, &master_set);
          Client new_client(newClient,NOTREGISTERED);
          onlineClients.push_back(new_client);
        }
        else
        { // handle commands from clients
          // recieve command from client
          msg.clear();
          if ((byteCount = recv(i, buffer, BUFFER_SIZE, 0)) == -1)
          {
            // client has diconnected (close the console), add a log;
            cout << "rcv go -1 from: " << i << endl;
            FD_CLR(i, &master_set);
            // clear online client
            // Client client = findClient(i);
            // if (client.index != NOTREGISTERED)
            // {
            //   onlineClients.erase(onlineClients.begin() + client.index);
            // }
            int temp_index = findClientIndex(i);
            if (temp_index != NOTREGISTERED)
            {
              onlineClients.erase(onlineClients.begin() + temp_index);
            }
            continue;
          }
          if( byteCount == 0) {
            cout << "rcv go 0 from: " << i << endl;
            FD_CLR(i, &master_set);
            // clear online client
            // Client client = findClient(i);
            // if (client.index != NOTREGISTERED)
            // {
            //   onlineClients.erase(onlineClients.begin() + client.index);
            // }
            int temp_index = findClientIndex(i);
            if (temp_index != NOTREGISTERED)
            {
              onlineClients.erase(onlineClients.begin() + temp_index);
            }
            continue;
          }
          // the little code below is the same as above
          // if(msg.size() == 0) {
          //     continue;
          // }
          //  if there is no one then recv return 0 i guess so we
          // should consider this.
          msg = string(buffer);
          cout << "msg is: " << msg << endl;
          // sleep(2);
          // handle commands
          // client hasnt logged in yet
          // Client client = findClient(i);
          Client client;
          int temp_index = findClientIndex(i);
          if (temp_index == NOTREGISTERED){
              cout << "client not found" << endl;
          } else {
              client = onlineClients[temp_index];
          }
          if (client.index == NOTREGISTERED)
          {
            if (client.commandID == NOTREGISTERED) {
              // sockekt recognition and setting commandID if correct
              client.socket_fd = i;
              client.isAdmin = false; 
              if( msg == "0")
              { // command1 : signup
                client.commandID = 0;
              } else if (msg == "1")
              { // command2 : login
                client.commandID = 1;
              } else 
              {
                msg = get_error(503);
                logEvent(SYSTEM, 13, msg);
              }
            } else
            { // client has entered signup/login
              if(msg == "quit")
              {
                client.commandID = NOTREGISTERED;
                client.command = "";
                client.socket_fd = i;
                client.isAdmin = false;
              } else if( client.commandID == 0) 
              { // command1 : signup : failed - pending - successfull
                Status signUpStatus = signUp(client, msg);
                if( signUpStatus == Status::ACCEPTED) {
                  // create a new user
                  addUser(client.command);
                  // reset client
                  client = Client();
                  client.socket_fd = i;
                }
              } else if (client.commandID == 1)
              { // command2 : login
                login(client, msg);
              }
            }
            onlineClients[temp_index] = client;
          } else
          { // client has logged in
            // tokenize msg
            vector<string> tokens;
            tokens = tokenize(msg, ' ');
            int commandNum;
            if(client.commandID == NOTREGISTERED) {
              // msg to commandNum
              try {
                commandNum = stoi(tokens[0]);
              } catch (...) {             
                msg = get_error(503);
                // logEvent(SYSTEM, 13, msg);
                client.commandID = NOTREGISTERED;
                commandNum = NOTREGISTERED;
                // continue;
              }
              client.commandID = commandNum;
            } else {
              commandNum = client.commandID;
            }
            bool isCommon = true;
            cout << "line 293: commandNum is: " << commandNum << endl;
            // client (common commands)
            switch(commandNum) {
              case 0: // logout
                logout(client, msg);
                break;
              case 1: // view user info
                viewUserInfo(client, msg);
                break;
              case 2: // view room info
                viewRoomsInfo(client, msg ,tokens);
                break;
              case 3: // edit info
                editInfo(client,msg, tokens);
                break;
              default:
                isCommon = false;
                break;
            }
            if(!isCommon) {
              if (client.isAdmin) {
                // admin 
                switch (commandNum)
                {
                case 4: // view all usersInfo
                  viewAllUsersInfo(client, msg);
                  break;
                case 5: // banGuest
                  banGuestsFromRoom(client, msg, tokens);
                  break;
                case 6: // addroom
                  addRoom(client, msg, tokens);
                  break;
                case 7: // modify room
                  modifyRoom(client, msg, tokens);
                  break;
                case 8: // delete room
                  deleteRoom(client, msg, tokens);
                  break;
                case 9: // pass day
                  passDay(client, msg, tokens);
                  break;

                default: // error command
                  msg = get_error(503);
                  logEvent(SYSTEM, 13, msg);
                  break;                  
                }
              } else {
                // user
                switch (commandNum)
                {
                case 4: // book room
                  bookRoom(client, msg, tokens);
                  break;
                case 5: // cancel reserve
                  cancelReserve(client, msg, tokens);
                  break;
                case 6: // leave room
                  leaveRoom(client, msg, tokens);
                  break;
                default: // error command
                  msg = get_error(503);
                  logEvent(SYSTEM, 13, msg);
                  break;

                }
              }
            }
            // reset commandID
            client=Client(client.socket_fd,client.index);
            onlineClients[temp_index] = client;
          }
          // check client has left
          // cout << msg.substr(0,50) << "\t is to send to clinet\n"<< endl; debug
          // send size of msg first to the client
          int msgSize = msg.length() + 1;
          cout << "369: msg is: " << msg << endl;
          // convert int to  constant c string
          if ((byteCount = send(i, to_string(msgSize).c_str(),
            to_string(msgSize).length()+1, 0)) == -1) 
          {
            cout << i << ": has error got -1 on send" << endl;
            FD_CLR(i, &master_set);
            // clear online client
            // Client client = findClient(i);
            int temp_index = findClientIndex(i);
            if (temp_index != NOTREGISTERED)
            {
              onlineClients.erase(onlineClients.begin() + temp_index);
            }
            continue;
          }
          memset(buffer, 0, 4);
          recv(i, buffer, 3, 0);
          if ((byteCount = send(i, msg.c_str(), msg.length() + 1, 0)) == -1)
          {
            cout << i << ": has error got -1 on send" << endl;
            FD_CLR(i, &master_set);
            // clear online client
            // Client client = findClient(i);
            int temp_index = findClientIndex(i);
            if (temp_index != NOTREGISTERED)
            {
              onlineClients.erase(onlineClients.begin() + temp_index);
            }
            continue;
          }
          if( byteCount == 0) {
            cout << "send got 0 from: " << i << endl;
            FD_CLR(i, &master_set);
            // clear online client
            // Client client = findClient(i);
            int temp_index = findClientIndex(i);
            if (temp_index != NOTREGISTERED)
            {
              onlineClients.erase(onlineClients.begin() + client.index);
            }
            continue;
          }
        }
      }
    }
  }
}

// server uttilities
// Client& Server::findClient(int socket_fd)
// {
//   for (auto &c : onlineClients)
//   {
//     if (c.socket_fd == socket_fd)
//     {
//       return c; // check it is refrence
//     }
//   }
//   // Client c;
//   // c.index = NOTREGISTERED;
//   // c.commandID = NOTREGISTERED;
//   // return c;
// }

// retrun index of onlineClients vector
int Server::findClientIndex(int socket_fd)
{
  for (int i = 0; i < onlineClients.size(); i++)
  {
    if (onlineClients[i].socket_fd == socket_fd)
    {
      return i;
    }
  }
  return NOTREGISTERED;
}
// find user or admin
// password, index, isAdmin

MiniClient Server::findUserAdminByName(string name) {
  MiniClient client;
  client.index = NOTREGISTERED;
  for (int i = 0; i < admins.size(); i++)
  {
    if (admins[i].getAdminName() == name)
    {
      client.index = i;
      client.isAdmin = true;
      client.password = admins[i].getAdminPassword();
      return client;
    }
  }
  for (int i = 0; i < users.size(); i++)
  {
    if (users[i].getClientName() == name)
    {
      client.index = i;
      client.isAdmin = false;
      client.password = users[i].getClientPassword();
      return client;
    }
  }
  return client;
}
// repeated name
bool Server::isRepeatedName(string name) {
  for (int i = 0; i < admins.size(); i++)
  {
    if (admins[i].getAdminName() == name)
      return true;
  }
  for (int i = 0; i < users.size(); i++)
  {
    if (users[i].getClientName() == name)
      return true;
  }
  return false;
}
// find unique user id
int Server::findUniqueID() {
  // random int 
  int id = 0;
  id = users.size() + admins.size() + 1;
  while (true)
  {
    bool flag = true;
    for (int i = 0; i < admins.size(); i++)
    {
      if (admins[i].getId() == id)
      {
        flag = false;
        break;
      }
    }
    if (flag)
    {
      for (int i = 0; i < users.size(); i++)
      {
        if (users[i].getId() == id)
        {
          flag = false;
          break;
        }
      }
    }
    if (flag)
      return id;
    id++;
  }
}
// create new user
void Server::addUser(string args) {
  // parse args 
  vector<string> tokens;
  tokens = tokenize(args);
  // find unique id
  int uniqueID = findUniqueID();
  User user(uniqueID, tokens[0], tokens[1], stoi(tokens[2]),
   tokens[3], tokens[4]);
  users.push_back(user);
  // LOG
  logEvent(USER, 3, "user " + tokens[0] + " added", user.getId());
}
// find room by name
int Server::getRoomIndexByRoomNum(int roomNum) {
    for (int i = 0; i < rooms.size(); i++)
    {
        if (rooms[i].getNumber() == roomNum) {
            return i;
        }
    }
    return -1;
}
void Server::writeJsonFile() {
  // overwrite to json file
  // overwrite rooms to file
  ofstream o(this->roomsPath);
  json j;
  j["rooms"] = rooms;
  o << setw(4) << j << endl;
  o.close();
  // clear j
  j.clear();
  // overwrite admins and users to file
  o.open(this->usersPath);
  j["users"] = users;
  j["admins"] = admins;
  o << setw(4) << j << endl;
  o.close();
}

// commands
// client
Status Server::signUp(Client& client, string& msg) {
    // msg shouldnt contain \n
    string sendMsg;
    boost::trim(msg);
    Status status = Status::PENDDING;
    if(msg.size() == 0) {
      status = Status::REJECTED;
      int errorNum = 503;
      logEvent(SYSTEM, 5, get_error(errorNum));
      msg = get_error(errorNum);
      return status;
    }
    bool flagValid = true;
    switch (client.argsNum)
      {
      case 0: // username
        flagValid = isAlphaNum(msg);
        if (!flagValid) {
          status = Status::REJECTED;
          int errorNum = 503;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);
          return status;
        }
        flagValid = isRepeatedName(msg);
        if (flagValid) {
          status = Status::REJECTED;
          int errorNum = 451;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);                    
          return status;
        }
        sendMsg = get_error(311);
        break;
      case 1: // password
        flagValid = isAlphaNum(msg) && strongPassword(msg);
        if (!flagValid) {
          status = Status::REJECTED;
          int errorNum = 503;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);
          return status;
        }
        sendMsg = get_error(200);
        break;
      case 2: // balance
        flagValid = isNumber(msg);
        if (!flagValid) {
          status = Status::REJECTED;
          int errorNum = 503;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);
          return status;
        }
        sendMsg = get_error(200);
        break;
      case 3: // phone
        flagValid = isPhoneNumber(msg);
        if (!flagValid) {
          status = Status::REJECTED;
          int errorNum = 503;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);
          return status;
        }
        sendMsg = get_error(200);
        break;
      case 4: // address
        flagValid = isAddress(msg);
        if (!flagValid) {
          status = Status::REJECTED;
          int errorNum = 503;
          logEvent(SYSTEM, 5, get_error(errorNum));
          msg = get_error(errorNum);
          return status;
        }
        status = Status::ACCEPTED;
        sendMsg = get_error(231);
        break;
      }
      client.argsNum++;
      client.command += (msg+"\n");
      msg = sendMsg;
      return status;
  }

void Server::login(Client& client, string& msg) {
  string sendMsg;
  vector<string> tokens;
  tokens = tokenize(msg, ' ');
  if(tokens.size() != 2) {
    int errorNum = 503;
    logEvent(SYSTEM, 24, get_error(errorNum));
    msg = get_error(errorNum);
    client.commandID = NOTREGISTERED;
    return;
  }
  string username = tokens[0];
  string password = tokens[1];
  // check invalid username or password
  MiniClient cur_client = findUserAdminByName(username);
  if (cur_client.index == NOTREGISTERED || cur_client.password != password) { 
    int errorNum = 430;
    logEvent(SYSTEM, 24, get_error(errorNum));
    msg = get_error(errorNum);
    client.commandID = NOTREGISTERED;
    return;
  }
  // client has logged in successfully
  client.index = cur_client.index;
  client.isAdmin = cur_client.isAdmin;
  client.argsNum = 0;
  client.command = "";
  client.commandID = NOTREGISTERED;
  int errorNum = 230;
  // if client is admin then logEvent(ADMIN, 6, get_error(errorNum),user_id)
  //  else logEvent(USER, 6, get_error(errorNum),user_id)
  int user_id = (client.isAdmin) ? admins[client.index].getId() : users[client.index].getId();
  logEvent((client.isAdmin) ? ADMIN : USER, 6, get_error(errorNum), user_id);
  
  msg = "successful " + to_string(client.isAdmin);
  
}

void Server::logout(Client& client, string& msg) {
  int errorNum = 201;
  int user_id = (client.isAdmin) ? admins[client.index].getId() : users[client.index].getId();
  logEvent((client.isAdmin) ? ADMIN : USER, 4, get_error(errorNum), user_id);
  client = Client(client.socket_fd, NOTREGISTERED);
  msg = get_error(errorNum);
}  

void Server::viewUserInfo(Client& client, string& msg) {
  int errorNum = 110;
  int user_id = (client.isAdmin) ? admins[client.index].getId() : users[client.index].getId();
  logEvent((client.isAdmin) ? ADMIN : USER, 7, get_error(errorNum), user_id);
  if (client.isAdmin) 
  {
    msg = "Admin ID: " + to_string(admins[client.index].getId()) 
    + "\nAdmin Name: " + admins[client.index].getAdminName() 
    + "\nAdmin Password: " + admins[client.index].getAdminPassword();
  } 
  else 
  {
    msg = "User ID: " + to_string(users[client.index].getId()) 
    + "\nUser Name: " + users[client.index].getClientName() 
    + "\nUser Password: " + users[client.index].getClientPassword() 
    + "\nUser Balance: " + to_string(users[client.index].getClientBalance()) 
    + "\nUser Phone: " + users[client.index].getClientPhoneNumber() 
    + "\nUser Address: " + users[client.index].getClientAddress()
    + "\n"
    + "\nUser Booked Rooms: \n"
    + users[client.index].getReservedRooms();
  }
}

void Server::viewRoomsInfo(Client& client, string& msg, vector<string>& command) {
  // if command.size() == 2 and command[1] == "empty" then show available rooms else show all rooms
  // if client is admin then show booked users in each room also
  int errorNum = 110;
  int user_id = (client.isAdmin) ? admins[client.index].getId() : users[client.index].getId();
  if (command.size() == 2 && command[1] == "empty")
  {
    for (int i = 0; i < rooms.size(); i++)
    {
      msg = "Available Rooms Inforamtion:\n";
      if (rooms[i].getStatusString() == "empty")
      {
        msg += rooms[i].getRoomInfo();

        // if client is admin then show booked users in each room also
        if (client.isAdmin)
        {
          msg += "***Booked Users:***\n";
          msg += rooms[i].getBookedClientsString();
        } 
        msg += "\n*****************************\n";
      }
    }
  }
  else if ((command.size() == 1) || (command.size() == 2 && command[1] == "all"))
  {
    msg = "All Rooms Information:\n";
    for (int i = 0; i < rooms.size(); i++)
    {
      msg += rooms[i].getRoomInfo();
      // if client is admin then show booked users in each room also
      if (client.isAdmin)
      {
        msg += "***Booked Users:***\n";
        msg += rooms[i].getBookedClientsString();
      }
      msg += "\n*****************************\n";
    }
  }
  else
  {
    errorNum = 503;
    logEvent((client.isAdmin) ? ADMIN : USER, 8, get_error(errorNum),user_id);
    msg = get_error(errorNum);
    return;
  }

  logEvent((client.isAdmin) ? ADMIN : USER, 8, get_error(errorNum),user_id);

}

void Server::editInfo(Client& client, string& msg, vector<string>& command)
{
  // command[0] is command number; command[1] is new password; command[2] is new phone; command[3] is new address;
  // if client is admin then check if command contains <New Password>
  // if client is user then check if command contains <New Password> <Phone> <Address>
  // if client is admin then logEvent(ADMIN, 5, get_error(errorNum),user_id)
  // else logEvent(USER, 5, get_error(errorNum),user_id)
  msg = "";
  if(client.isAdmin) {
    // client is admin
    if(command.size() != 2) {
      // command is not valid
      int errorNum = 503;
      logEvent(ADMIN, 11, get_error(errorNum),admins[client.index].getId());
      msg = get_error(errorNum);
      return;
    }
    // check if new password is valid
    if(!isValidNewPassword(command[1], admins[client.index].getAdminPassword())) {
      // new password is not valid
      int errorNum = 401;
      logEvent(ADMIN, 11, get_error(errorNum),admins[client.index].getId());
      msg = get_error(errorNum);
      return;
    }
    // change password
    admins[client.index].setAdminPassword(command[1]);
    // log
    int errorNum = 312;
    logEvent(ADMIN, 11, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
  } else {
    bool sth_changed = false;
    //  client is user
    if(command.size() != 4) {
      // command is not valid
      int errorNum = 503;
      logEvent(USER, 11, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
      return;
    }
    // i thnk we should use less log events;
    if( command[1] != "n") {
      if(!isValidNewPassword(command[1], users[client.index].getClientPassword())) {
        // new password is not valid
        int errorNum = 401;
        logEvent(USER, 11, get_error(errorNum),users[client.index].getId());
        msg += get_error(errorNum);
      }else {
        // change password
        sth_changed = true;
        users[client.index].setClientPassword(command[1]);
      }
    }
    if(command[2] != "n") {
      // check if phone is valid and not equal to old phone
      if(!isPhoneNumber(command[2]) || command[2] == users[client.index].getClientPhoneNumber()) {
        // phone is not valid
        int errorNum = 401;
        logEvent(USER, 11, get_error(errorNum),users[client.index].getId());
        msg += get_error(errorNum);
      } else {
        // change phone
        sth_changed = true;
        users[client.index].setClientPhoneNumber(command[2]);
      }
    }
    if(command[3] != "n") {
      // check if address is valid and not equal to old address
      // replace * with space in ADDRESS string
      string address = command[3];
      for(int i = 0; i < address.length(); i++) {
        if(address[i] == '*') {
          address[i] = ' ';
        }
      }
      if(!isAddress(address) || address == users[client.index].getClientAddress()) {
        // address is not valid
        int errorNum = 401;
        logEvent(USER, 11, get_error(errorNum),users[client.index].getId());
        msg += get_error(errorNum);
      } else {
        // change address
        sth_changed = true;
        users[client.index].setClientAddress(address);
      }
    }
    if(sth_changed) {
      // log
      int errorNum = 312;
      logEvent(USER, 11, get_error(errorNum),users[client.index].getId());
      msg += get_error(errorNum);
    }
  }
}

// user
void Server::bookRoom(Client& client, string& msg,vector<string>& commands)
{
  int errorNum;
  // check if command contains <RoomNum> <NumOfBeds> <ReserveDate> <CheckOutDate>
  //  and checkoutdate > reservedate
  if(commands.size()!=5)
  {
    errorNum = 503;
    logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  if(!isNumber(commands[1]) || !isNumber(commands[2]) || !regex_match(commands[3],dateRegex)
   || !regex_match(commands[4],dateRegex) 
   || !validRangeDate(commands[3],commands[4],this->date))
  {
    errorNum = 503;
    logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  // get number of days
  int numOfDays = getNumOfDays(commands[3],commands[4]);
  // check if room number if valid
  errorNum = 101;
  int roomNum = stoi(commands[1]);
  for(int i=0; i<rooms.size() ; i++)
  {
    if(rooms[i].getNumber()==roomNum)
    {
      //check if user balance is valid to reserve this room
      int numOfBeds = stoi(commands[2]);
      if(numOfDays*numOfBeds*rooms[i].getPrice() > users[client.index].getClientBalance())
      {
        errorNum = 108;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      // check if number of beds is available on requested date
      string requestedCheckInDate = commands[3];
      string requestedCheckOutDate = commands[4];
      if(rooms[i].isRoomAvailable(requestedCheckInDate,requestedCheckOutDate,
      numOfBeds, users[client.index].getId()))
      {
        // reserve room
        // new a booked client
        // index is bookeedClients vector index in room
        // client.index is the index in users/admins vectors;
        int index = rooms[i].findClientbyId(users[client.index].getId());
        if(index == -1) {
          bookedClient* new_client = new bookedClient(users[client.index].getId(),
          numOfBeds,requestedCheckInDate,requestedCheckOutDate,roomNum);
          // add client to room
          rooms[i].addBookedClient(new_client);
          // add room to user
          users[client.index].addReservedRoom(new_client);
        } else { // client has canceled compeletly and want to rerasarve
          rooms[i].editBookedClient(index, numOfBeds, requestedCheckInDate, requestedCheckOutDate);

        }

        // update user balance
        users[client.index].setClientBalance(users[client.index].getClientBalance()
         - numOfBeds*rooms[i].getPrice());

        // if reserv is for today then update room status
        if(this->date == requestedCheckInDate){
          // update room status
          rooms[i].updateStatus(numOfBeds);
        }
        errorNum = 110;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      else
      { // room not available
        errorNum = 107;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      // break definitely return before this line
    }
  }
  logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
  msg = get_error(errorNum);
}

void Server::cancelReserve(Client& client, string& msg,vector<string>& commands) {
  // the client can cancel his/her reservation only before the check-in date and recieve 
  // the half of the money; 
  msg = "";
  if(commands.size() == 1){
    // show future reserved rooms
    msg = users[client.index].showFutureReservations(this->date);
    // log
    logEvent(USER, 10, "successful!! send future reservations list",users[client.index].getId());
    
  }else if(commands.size() == 2 && commands[1] == "quit") {
      logEvent(USER, 10, "user quit the cancelReserve operation",users[client.index].getId());
      msg = "quit";
  } else if(commands.size() == 3){
      // delete numofbeds from room
      // check if room number and numofbeds is valid
      if(!isNumber(commands[1]) || !isNumber(commands[2])){
        int errorNum = 401;
        logEvent(USER, 10, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      int roomNum = stoi(commands[1]);
      int numOfBeds = stoi(commands[2]);
      // check if room number exist
      int errorNum = 101;   
      errorNum = users[client.index].cancelFutureReservation(roomNum,numOfBeds,date);
      if(errorNum == 110) {
        // cancelation was successful --> return half of the money:
        int roomIndex = getRoomIndexByRoomNum(roomNum);
        int pricePerBed = rooms[roomIndex].getPrice();
        int numOfDays =  users[client.index].getReserveDuration(roomNum);
        int refund = (pricePerBed * numOfDays) /2;
        users[client.index].updateBalance(refund);
      }
      // log
      logEvent(USER, 10, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
    } else {
    // invalid command
    int errorNum = 401;
    logEvent(USER, 10, get_error(errorNum),users[client.index].getId());
    msg = get_error(errorNum);
  }
}

void Server::leaveRoom(Client& client, string& msg,vector<string>& commands) 
{
    // check if room number is valid
    if(commands.size() != 2)
    {
      int errorNum = 503;
      logEvent(USER, 12, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
      return;
    }
    if(!isNumber(commands[1]))
    {
      int errorNum = 401;
      logEvent(USER, 12, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
      return;
    }
    int roomNum = stoi(commands[1]);
    // check if room number exist
    int errorNum = 503;
    int roomIndex = getRoomIndexByRoomNum(roomNum);
    // room not found
    if(roomIndex == -1) {
      // log
      
      logEvent(USER, 12, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
      return;      
    }
    errorNum = 102;
    Room& cur_room = rooms[roomIndex];
    // check if user is settling in this room at this moment
    // check if date is between checkin and checkout
    errorNum = 102;
    int numOfBeds = users[client.index].getCurReservedRoomNumOfBeds(roomNum,this->date);
    if(numOfBeds != -1)
    {
      errorNum = 413;
      // update room capacity
      cur_room.updateRoomCapacity(-numOfBeds);
      // delete reservation from user
      users[client.index].deleteReservedRoom(roomNum, this->date);
      // delete reservation from room
      cur_room.deleteBookedClient(users[client.index].getId());

      logEvent(USER, 12, get_error(errorNum),users[client.index].getId());
      msg = get_error(errorNum);
      return;
    }    
    logEvent(USER, 12, get_error(errorNum),users[client.index].getId());
    msg = get_error(errorNum);
}

// admin 
void Server::viewAllUsersInfo(Client& client, string& msg)
{
  // show users info and their reserved rooms
  int errorNum = 110;
  int user_id = admins[client.index].getId();
  logEvent(ADMIN, 14, get_error(errorNum),user_id);
  msg = "\t\tUSERS:\n";
  for (int i = 0; i < users.size(); i++)
  {
    msg += "User ID: " + to_string(users[i].getId()) + "\nUser Name: " +
     users[i].getClientName() + "\nUser Balance: " + to_string(users[i].getClientBalance()) +
      "\nUser Phone: " + users[i].getClientPhoneNumber() + "\nUser Address: " +
       users[i].getClientAddress() + "\n";
    
    msg += "Reserved Rooms:\n";
    msg += users[i].getReservedRooms();
    msg += "*****************************\n";
  }
  msg += "\t\tADMINS:\n";
  for (int i = 0; i < admins.size(); i++)
  {
    msg += "Admin ID: " + to_string(admins[i].getId()) + "\nAdmin Name: " +
     admins[i].getAdminName() + "\n";
    msg += "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  }

}

void Server::banGuestsFromRoom(Client& client, string& msg,vector<string>& commands)
{
  // coomand[1] is room number. by this command admin can remove all guests from a room
  // check if room number is valid
  if(commands.size() != 2)
  {
    int errorNum = 503;
    logEvent(ADMIN, 16, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int errorNum = 503;
  if(!isNumber(commands[1]))
  {
    logEvent(ADMIN, 16, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int roomNum = stoi(commands[1]);
  // check if room number exist
  int roomIndex = getRoomIndexByRoomNum(roomNum);
  // room not found
  if(roomIndex == -1) {
    // log
    errorNum = 101;
    logEvent(ADMIN, 16, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;      
  }
  Room& cur_room = rooms[roomIndex];
  // update room capacity
  cur_room.updateRoomCapacity(-cur_room.getMaxCapacity());
  // delete room from users
  for(int j=0; j<users.size(); j++)
  {
    users[j].deleteReservedRoom(roomNum, this->date);
  }
  // delete all reservations from rooms
  cur_room.deleteAllBookedClientsbyDate(this->date);
  errorNum = 413;

  logEvent(ADMIN, 16, get_error(errorNum) + to_string(roomNum),admins[client.index].getId());
  msg = get_error(errorNum);
}

void Server::addRoom(Client& client, string& msg,vector<string>& commands)
{
  // <RoomNum> <Max Capacity> <Price>
  // add room to rooms and check whether it is valid or not and room number is unique
  // if valid add it to rooms
  // check if room number is valid
  if(commands.size() != 4)
  {
    int errorNum = 503;
    logEvent(ADMIN, 17, get_error(errorNum) + " -> room number",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int errorNum = 503;
  if(!isNumber(commands[1]))
  {
    errorNum = 503;
    logEvent(ADMIN, 17, get_error(errorNum) + " -> room number",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int roomNum = stoi(commands[1]);
  // check if room number exist
  int roomIndex = getRoomIndexByRoomNum(roomNum);
  if(roomIndex != -1) {
    //room found, log
    errorNum = 111;
    logEvent(ADMIN, 17, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;      
  }
  // room not found
  errorNum = 104; // if add was successful
  // check if max capacity is valid
  if(!isNumber(commands[2]))
  {
    errorNum = 503;
    logEvent(ADMIN, 17, get_error(errorNum) + " -> max capacity",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int maxCapacity = stoi(commands[2]);
  // check if price is valid
  if(!isNumber(commands[3]))
  {
    errorNum = 503;
    logEvent(ADMIN, 17, get_error(errorNum) + " -> price",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int price = stoi(commands[3]);
  // add room to rooms
  Room room(roomNum,maxCapacity,price);
  rooms.push_back(room);

  logEvent(ADMIN, 17, get_error(errorNum),admins[client.index].getId());
  msg = get_error(errorNum);
}


void Server::modifyRoom(Client& client, string& msg,vector<string>& commands)
{
  // <RoomNum> <Max Capacity> <Price>
  // modify changes a room configuration if it is valid

  // check if room number is valid
  if(commands.size() != 4)
  {
    int errorNum = 503;
    logEvent(ADMIN, 18, get_error(errorNum), admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int errorNum = 503;
  if(!isNumber(commands[1]))
  {
    errorNum = 503;
    logEvent(ADMIN, 18, get_error(errorNum) + " -> room number",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int roomNum = stoi(commands[1]);
  // check if room number exist
  int roomIndex = getRoomIndexByRoomNum(roomNum);
  // room not found
  if(roomIndex == -1) {
    // log
    errorNum = 101;
    logEvent(ADMIN, 18, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;      
  }
  Room& cur_room = rooms[roomIndex];
  errorNum = 101;
  // check if max capacity is valid
  if(!isNumber(commands[2]))
  {
    errorNum = 503;
    logEvent(ADMIN, 18, get_error(errorNum) + " -> max capacity",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int maxCapacity = stoi(commands[2]);
  // check whether max capaviry is >= current capacity and return error 109 if not
  if(maxCapacity < cur_room.getCurrentCapacity())
  {
    errorNum = 109;
    logEvent(ADMIN, 18, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  // check if price is valid
  if(!isNumber(commands[3]))
  {
    errorNum = 503;
    logEvent(ADMIN, 18, get_error(errorNum) + " -> price",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int price = stoi(commands[3]);
  // modify room
  cur_room.setPrice(price);
  cur_room.setMaxCapacity(maxCapacity);
  errorNum = 105;
  logEvent(ADMIN, 18, get_error(errorNum),admins[client.index].getId());
  msg = get_error(errorNum);
  return;
  
  
  logEvent(ADMIN, 18, get_error(errorNum),admins[client.index].getId());
  msg = get_error(errorNum);
}

void Server::deleteRoom(Client& client, string& msg,vector<string>& commands)
{
  // <RoomNum>
  // delete room from rooms if it is valid and if it was not reserved in any date

  // check if room number is valid
  if(commands.size() != 2)
  {
    int errorNum = 503;
    logEvent(ADMIN, 19, get_error(errorNum), admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int errorNum = 503;
  if(!isNumber(commands[1]))
  {
    errorNum = 503;
    logEvent(ADMIN, 19, get_error(errorNum) + " -> room number",admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int roomNum = stoi(commands[1]);
  // check if room number exist
  int roomIndex = getRoomIndexByRoomNum(roomNum);
  // room not found
  if(roomIndex == -1) {
    // log
    errorNum = 101;
    logEvent(ADMIN, 19, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;      
  }
  Room& cur_room = rooms[roomIndex];
  errorNum = 101;
  // check if room was reserved in any date
  if(cur_room.hasReservation())
  {
    errorNum = 109;
    logEvent(ADMIN, 19, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  // delete room from rooms
  rooms.erase(rooms.begin()+roomIndex);
  errorNum = 106;
  logEvent(ADMIN, 19, get_error(errorNum),admins[client.index].getId());
  msg = get_error(errorNum);

}


void Server::passDay(Client& client, string& msg,vector<string>& commands) {
  int increaseDate;
  if(commands.size() != 2)
  {
    int errorNum = 503;
    logEvent(ADMIN, 15, get_error(errorNum), admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  int errorNum = 503;
  if(!isNumber(commands[1]))
  {
    errorNum = 503;
    logEvent(ADMIN, 15, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  increaseDate = stoi(commands[1]);
  // check if increaseDate is valid
  if(increaseDate < 1)
  {
    errorNum = 503;
    logEvent(ADMIN, 15, get_error(errorNum),admins[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  // pass day
  this-> date = convertSecondsToDate(convertToDate(this->date) + increaseDate * ONEDAYSECONDS); 
  //  delete cancled reservations
  //  capaacity of rooms and status
  //  check every room and update them if neccesary
  for (auto& user : users) {
    user.passDay(this->date); 
  }
  for (auto& room : rooms) {
    room.passDay(this->date); 
  } 
  // log
  errorNum = 110;
  logEvent(ADMIN, 15, get_error(errorNum)+ " days passed: "+commands[1],admins[client.index].getId());
  msg = get_error(errorNum);
}



//  at client side in 5 we should check sth,
//  first only "5", second time 5 <roomNum> <num>;
// remember to trim each argument before concatenating in client side;

int main() {
  Server server;
  // cout << "Server is running..." << endl;
  // sleep(5);
  server.run();
  return 0;
}