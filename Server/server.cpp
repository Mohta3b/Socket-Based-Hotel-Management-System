#include "server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <arpa/inet.h>
#include "../Server/Log.hpp"
#include "../Server/utility.hpp"
#include "../Server/Errors.hpp"
#include <boost/algorithm/string.hpp>

const int BUFFER_SIZE = 1024;
const int NOTREGISTERED = -1;


int Server::acceptClient(int server_fd = -2)
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

void Server::readRoomsUserFiles(string roomsPath = "RoomsInfo.json", string usersPath = "UsersInfo.json")
{
  json rooms = readJsonFile(roomsPath);
  json clients = readJsonFile(usersPath);
  this->users = clients["users"];
  this->rooms = rooms["rooms"];
  this->admins = clients["admins"];
}

void Server::setDate() {
  // clear terminal screen
  system("clear");
  cout << "please enter server start date" << endl;
  cout << "setTime <Date Time>\nsetTime ";
  string TempDate;
  cin >> TempDate;
  // check if date is valid with regex dd-mm-yyyy
  if (!regex_match(TempDate, dateRegex)) {
    cout << "invalid date, please try again" << endl;
    logEvent(SYSTEM, 23, get_error(401));
  } else {
    this->date = TempDate;
    logEvent(SYSTEM, 0, "Set Date was successfull");
    return;
  }
  setDate();
}

void Server::run()
{
  fd_set master_set, working_set;
  FD_ZERO(&master_set);
  FD_ZERO(&working_set);
  int byteCount;
  // setup server
  this->serverFd = setupServerSocket();
  if (serverFd < 0)
  {
    logEvent(SYSTEM, 22, "Error in tcp setup!");
    exit(-1);
  }
  string msg = "Server is running\t";
  logEvent(SYSTEM, 0, msg);
  setDate();

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
          Client new_client;
          onlineClients.push_back(new_client);
        }
        else
        { // handle commands from clients
          // recieve command from client
          msg.clear();
          if ((byteCount = recv(i, buffer, BUFFER_SIZE, 0)) == -1)
          {
            cout << i << "is hooooshdoooded no to rcv from" << endl;
            FD_CLR(i, &master_set);
            continue;
          }
          msg = string(buffer);
          if(msg.size() == 0) {
              continue;
          }
          //  if there is none then recv return 0 i guess so we
          // should consider this.
          // handle commands

          // client hasnt logged in yet
          Client client = findClient(i);
          if (client.index == NOTREGISTERED)
          {
            if (client.commandID == NOTREGISTERED) {
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
                logEvent(SYSTEM, 13, msg);
                client.commandID = NOTREGISTERED;
                continue;
              }
              client.commandID = commandNum;
            } else {
              commandNum = client.commandID;
            }
            bool isCommon = true;
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
                editInfo();
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
                  viewAllUsersInfo();
                  break;
                case 5: // banGuest
                  banGuest();
                  break;
                case 6: // addroom
                  addRoom();
                  break;
                case 7: // modify room
                  modifyRoom();
                  break;
                case 8: // delete room
                  deleteRoom();
                  break;
                case 9: // pass day
                  passDay();
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
                  bookRoom();
                  break;
                case 5: // cancel reserve
                  cancelReserve();
                  break;
                case 6: // leave room
                  leaveRoom();
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
          }

          // check client has left
          msg = string(buffer);
          msg += " server is very happy tp meet you\n";
          cout << msg << "\t is to send to clinet\n"
               << endl;
          if ((byteCount = send(i, msg.c_str(), msg.length() + 1, 0)) == -1)
          {
            cout << i << " is hooooshdoooded no one to send to" << endl;
            FD_CLR(i, &master_set);
            continue;
          }

        }
      }
    }
  }
}

// server uttilities
Client &Server::findClient(int socket_fd)
{
  for (auto &c : onlineClients)
  {
    if (c.socket_fd == socket_fd)
    {
      return c; // check it is refrence
    }
  }
  Client c;
  c.index = NOTREGISTERED;
  c.commandID = NOTREGISTERED;
  return c;
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
        sendMsg = get_error(200);
        break;
      }
      client.argsNum++;
      client.command += (msg+"\n"); //"  2232\n std\nf g l l \ndfggfh\ndfg  \n";
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
    return;
  }
  client.index = cur_client.index;
  client.isAdmin = cur_client.isAdmin;
  client.argsNum = 0;
  client.command = "";
  client.commandID = NOTREGISTERED;
  int errorNum = 230;
  // if client is admin then logEvent(ADMIN, 6, get_error(errorNum),user_id) else logEvent(USER, 6, get_error(errorNum),user_id)
  int user_id = (client.isAdmin) ? admins[client.index].getId() : users[client.index].getId();
  logEvent((client.isAdmin) ? ADMIN : USER, 6, get_error(errorNum), user_id);

  msg = get_error(errorNum);
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

void Server::viewRoomsInfo(Client& client, string& msg, std::vector<string>& command) {
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
        msg += "Room Number: " + to_string(rooms[i].getNumber()) 
        + "\nRoom Price: " + to_string(rooms[i].getPrice()) 
        + "\nRoom Current Residents: " + to_string(rooms[i].getCurrentCapacity()) 
        + "\nRoom Status: " + rooms[i].getStatusString() + " ( "
        + to_string(rooms[i].getMaxCapacity()-rooms[i].getCurrentCapacity()) 
        + " / " + to_string(rooms[i].getMaxCapacity()) + " )\n";

        // if client is admin then show booked users in each room also
        if (client.isAdmin)
        {
          msg += "***Booked Users:***\n";
          for (int j = 0; j < rooms[i].getBookedClients().size(); j++)
          {
            msg += "User ID: " + to_string(rooms[i].getBookedClients()[j].getId()) 
            + "\nNumber of Reserved Beds: " + to_string(rooms[i].getBookedClients()[j].getNumberofBeds()) 
            + "\nReservervation Date: " + rooms[i].getBookedClients()[j].getReserveDate() 
            + "\nCheck Out Date: " + rooms[i].getBookedClients()[j].getCheckoutDate() + "\n";
          }
        } 
        msg += "\n*****************************\n";
      }
    }
  }
  else if ((command.size() == 1) || (command.size() == 2 && command[1] == "all"))
  {
    msg = "All Rooms Information:";
    for (int i = 0; i < rooms.size(); i++)
    {
      msg += "Room Number: " + to_string(rooms[i].getNumber()) + "\nRoom Price: " + to_string(rooms[i].getPrice()) + "\nRoom Current Residents: " + to_string(rooms[i].getCurrentCapacity()) + "\nRoom Status: " + rooms[i].getStatusString() + " ( " + to_string(rooms[i].getMaxCapacity()-rooms[i].getCurrentCapacity()) + " / " + to_string(rooms[i].getMaxCapacity()) + " )" + "\n*****************************\n";
      // if client is admin then show booked users in each room also
      if (client.isAdmin)
      {
        msg += "***Booked Users:***\n";
        for (int i = 0; i < rooms.size(); i++)
        {
          for (int j = 0; j < rooms[i].getBookedClients().size(); j++)
          {
            msg += "User ID: " + to_string(rooms[i].getBookedClients()[j].getId()) + "\nNumber of Reserved Beds: " + to_string(rooms[i].getBookedClients()[j].getNumberofBeds()) + "\nReservervation Date: " + rooms[i].getBookedClients()[j].getReserveDate() + "\nCheck Out Date: " + rooms[i].getBookedClients()[j].getCheckoutDate() + "\n";
          }
        }
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

void editInfo(Client& client, string msg)
{
  

}

// user
void Server::bookRoom(Client& client, string msg,vector<string>& commands)
{
  int errorNum;
  // check if command contains <RoomNum> <NumOfBeds> <ReserveDate> <CheckOutDate> and checkoutdate > reservedate
  if(!isNumber(commands[1]) || !isNumber(commands[2]) || !regex_match(commands[3],dateRegex) || !regex_match(commands[4],dateRegex) 
  || validRangeDate(commands[3],commands[4]))
  {
    errorNum = 503;
    logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
    msg = get_error(errorNum);
    return;
  }
  // check if room number if valid
  errorNum = 101;
  int roomNum = stoi(commands[1]);
  for(int i=0; i<rooms.size() ; i++)
  {
    if(rooms[i].getNumber()==roomNum)
    {
      //check if user balance is valid to reserve this room
      int numOfBeds = stoi(commands[2]);
      if(numOfBeds*rooms[i].getPrice() > users[client.index].getClientBalance())
      {
        errorNum = 108;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      // check if number of beds is available on requested date
      string requestedCheckInDate = commands[3];
      string requestedCheckOutDate = commands[4];
      if(rooms[i].isAvailable(requestedCheckInDate,requestedCheckOutDate,numOfBeds))
      {
        // reserve room
        bookedClient new
        
        
        // update user balance
        users[client.index].setClientBalance(users[client.index].getClientBalance() - numOfBeds*rooms[i].getPrice());

        errorNum = 110;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      else
      {
        errorNum = 107;
        logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
        msg = get_error(errorNum);
        return;
      }
      break;
    }
  }
  logEvent(USER, 9, get_error(errorNum),users[client.index].getId());
  msg = get_error(errorNum);
}
void Server::cancelReserve();
void Server::leaveRoom();

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
    msg += "User ID: " + to_string(users[i].getId()) + "\nUser Name: " + users[i].getClientName() + "\nUser Balance: " + to_string(users[i].getClientBalance()) + "\nUser Phone: " + users[i].getClientPhoneNumber() + "\nUser Address: " + users[i].getClientAddress() + "\n";
    
    msg += "Reserved Rooms:\n";
    msg += users[i].getReservedRooms();
    msg += "*****************************\n";
  }
  msg += "\t\tADMINS:\n";
  for (int i = 0; i < admins.size(); i++)
  {
    msg += "Admin ID: " + to_string(admins[i].getId()) + "\nAdmin Name: " + admins[i].getAdminName() + "\n";
    msg += "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  }

}


void Server::banGuest();
void Server::addRoom();
void Server::modifyRoom();
void Server::deleteRoom();
void Server::passDay();

// diff commands
void Server::editInfo();

//  master set as server private property, no need because the client can use the same port
