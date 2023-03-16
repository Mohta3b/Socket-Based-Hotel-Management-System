#pragma once
#include "./client.hpp"
#include "../Server/Errors.hpp"

int Client::connectServer()
{
  int fd;
  struct sockaddr_in server_address;
  fd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = inet_addr(hostName.c_str());

  if (connect(fd, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0)
  { // checking for errors
    printf("Error in connecting to server\n");
    return -1;
  }
  return fd;
}

void Client::run()
{

  cout << "*** Welcome to Hotel Management System 505! ***" << endl;
  socketFd = connectServer();
  if (socketFd < 0)
  {
    perror("server tcp socket failed");
    exit(EXIT_FAILURE);
  }
  string msg;
  vector<string> args;
  while (true)
  {
    showMenu();
    // get coomandId
    string input_msg;
    string output_msg;
    input_msg = get_input();
    args = tokenize(input_msg, ' ');

    if (args.size() == 0)
    { // if Enter is pressed
      // clear terminal screen
      system("clear");
      continue;
    }
    else if (this->status == ClientStatus::NOT_LOGIN)
    {
      // signup <username><password><balance><phone><address> / login<username><password>
      msg = input_msg;
      send(socketFd, msg.c_str(), msg.length() + 1, 0);
      output_msg = RecieveFromServer();
      if (output_msg == "0") // signup
      {
        cout << "Note: You can cancel this operation by typing 'quit' in any step." << endl;
        string Help = commandsHelp["signup"] + "\nEnter your username:\n>>";
        if(getCommand(Help,311)){
          continue;
        }
        Help = commandsHelp["signup"] + "\nEnter your password:\n>>";
        if(getCommand(Help,200)){
          continue;
        }
        Help = commandsHelp["signup"] + "\nEnter your balance:\n>>";
        if(getCommand(Help,200)){
          continue;
        }
        Help = commandsHelp["signup"] + "\nEnter your phone:\n>>";
        if(getCommand(Help,200))
          continue;
        Help = commandsHelp["signup"] + "\nEnter your address:\n>>";
        if(getCommand(Help,231))
          continue;
        system("clear");
        cout << "Your account has been created successfully.\n";
        PressEntertoContinue();
      }
      else if (output_msg == "1") // login
      {
        string Help = commandsHelp["login"] 
        + "\nEnter your Username and Password(<username> <password>):\n>>";
        cout << Help << endl;
        input_msg = get_input();
        send(socketFd, input_msg.c_str(), input_msg.length() + 1, 0);
        output_msg = RecieveFromServer();
        if (output_msg == get_error(430) || output_msg == get_error(503)) // failed to login
        {
          cout << output_msg << endl;
          PressEntertoContinue();
          continue;
        }
        else if (output_msg == "successful 1") // admin
        {
          this->status = ClientStatus::ADMIN;
        }
        else if (output_msg == "successful 0") // user
        {
          this->status = ClientStatus::USER;
        }
        system("clear");
        cout << "You have successfully logged in.\n";
        PressEntertoContinue();
      }
      else // invalid command
      {
        cout << output_msg << endl;
        PressEntertoContinue();
      }
    } // client has logged in
    else if (this->status == ClientStatus::USER)
    {
      msg = input_msg;
      string Help;
      string password,phone,address,newInfo;
      commandId = stoi(args[0]);
      // user commands
      switch (commandId)
      {
      case 3:// edit info(we should convert spaces to stars in address)
        // send(socketFd, msg.c_str(), msg.length() + 1, 0);
        // output_msg = RecieveFromServer();
        // cout << output_msg << endl;
        // clear terminal screen
        system("clear");
        cout << commandsHelp["edit_info"] << endl;
        cout << "Enter your new password(Type 'n' if you don't want to change this field!):\n>>" << endl;
        password = get_input();
        cout << "Enter your new phone(Type 'n' if you don't want to change this field!):\n>>" << endl;
        phone = get_input();
        cout << "Enter your new address(Type 'n' if you don't want to change this field!):\n>>" << endl;
        address = get_input();
        convertAddress(address);
        newInfo = password + " " + phone + " " + address;
        send(socketFd, newInfo.c_str(), newInfo.length() + 1, 0);
        output_msg = RecieveFromServer();
        cout << output_msg << endl;
        PressEntertoContinue();
        break;
      
      case 5:// cancel reservation (first server will send us the list of reservations and then we should choose one of them)
        send(socketFd, msg.c_str(), msg.length() + 1, 0);
        output_msg = RecieveFromServer();
        cout << output_msg << endl;
        Help =" Enter the room number you want to cancel from the above list and the number of beds(<roomNum> <numOfBeds>):";
        cout << Help << endl;
        Help = output_msg + Help + "\n>>";
        getCancelCommand(Help,110);
        cout << "Your reservation has been canceled successfully.\n";
        PressEntertoContinue();
        break;

      default:
        send(socketFd, msg.c_str(), msg.length() + 1, 0);
        output_msg = RecieveFromServer();
        // logout successfully
        if(output_msg == get_error(201)) {
          this->status = ClientStatus::NOT_LOGIN;
        }
        cout << output_msg << endl;
        PressEntertoContinue();
        break;
      }
    }
    else if (this->status == ClientStatus::ADMIN)
    {
      // admin commands
      msg = input_msg;
      commandId = stoi(args[0]);
      send(socketFd, msg.c_str(), msg.length() + 1, 0);
      output_msg = RecieveFromServer();
      // logout successfully
      if(output_msg == get_error(201)) {
        this->status = ClientStatus::NOT_LOGIN;
      }
      cout << output_msg << endl;
      PressEntertoContinue();
    }
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
// show menu
void Client::showMenu()
{
  if (this->status == ClientStatus::NOT_LOGIN)
  {
    cout << "Please enter your choice:" << endl;
    cout << "0. Sign up" << endl;
    cout << "1. Login" << endl;
  }
  else if (this->status == ClientStatus::USER)
  {
    cout << "*** User Menu ***" << endl;
    cout << "Please enter your choice:" << endl;
    cout << "0. Logout" << endl;
    cout << "1. View Account Information" << endl;
    cout << "2. View Room Information (<2> <full/empty>)" << endl;
    cout << "3. Edit Account Information" << endl;
    cout << "4. Reserve Room (<4> <room number> <number of beds> <check in date> <check out date>)" << endl;
    cout << "5. Cancel Reservation (<5> <room number> <number of beds>)" << endl;
    cout << "6. Leave Room (<6> <room number>)" << endl;
  }
  else if (this->status == ClientStatus::ADMIN)
  {
    cout << "*** Admin Menu ***" << endl;
    cout << "Please enter your choice:" << endl;
    cout << "0. Logout" << endl;
    cout << "1. View Account Information" << endl;
    cout << "2. View Room Information (<2> <full/empty>)" << endl;
    cout << "3. Edit Account Information" << endl;
    cout << "4. View All Users Information" << endl;
    cout << "5. Ban Guests From Room (<5> <room number>)" << endl;
    cout << "6. Add Room (<6> <room number> <max capacity> <price per bed>)" << endl;
    cout << "7. Modify Room (<7> <new max capacity> <new price per bed>)" << endl;
    cout << "8. Remove Room (<8> <room number>)" << endl;
    cout << "9. Pass Time (<9> <number of days>)" << endl;
  }
  cout << endl << ">>";
}

// void Client::showCommandHelp(int commandId)
// {
//   if (this->status == ClientStatus::NOT_LOGIN)
//   {
//     switch (commandId)
//     {
//     case 0:
//       // signup
//       cout << commandsHelp.at("signup") << endl;
//       break;
//     case 1:
//       // login
//       cout << commandsHelp.at("login") << endl;
//       break;
//     default:
//       break;
//     }
//   }
//   else
//   {
//     // loggedin commands
//     if (this->status == ClientStatus::ADMIN)
//     {
//       // admin commands
//     }
//     else
//     {
//       // user commands
//     }
//   }
// }

string Client::RecieveFromServer()
{
  string msg;
  int n = recv(this->socketFd, buffer, BUFFER_SIZE, 0);
  if (n <= 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }
  int msg_size = atoi(buffer);
  memset(buffer, 0, BUFFER_SIZE);
  recv(this->socketFd, buffer, msg_size, 0);
  msg = string(buffer);
  return msg;
}

bool Client::getCommand(string Help, int errorCode){
  // it will get command from user and send it to server and do this until server send an error code
  string input_msg;
  string output_msg;
  while (true)
  {
    cout << Help;
    input_msg = get_input();
    send(socketFd, input_msg.c_str(), input_msg.length() + 1, 0);
    output_msg = RecieveFromServer();
    if (output_msg == get_error(errorCode) || (errorCode!=230 && output_msg == "quit"))
    {
      if(output_msg == "quit") {
        cout << "you have canceled this operation" << endl;
        PressEntertoContinue();
        return true;
      }
      return false;
    }
    cout << output_msg << endl;
    PressEntertoContinue();
  }
}

void Client::getCancelCommand(string Help, int errorCode){
  string input_msg;
  string output_msg;
  while (true)
  {
    cout << Help;
    input_msg = get_input();
    //  prefix "5" to the command to cancel it
    input_msg = "5 " + input_msg;
    send(socketFd, input_msg.c_str(), input_msg.length() + 1, 0);
    output_msg = RecieveFromServer();
    if (output_msg == get_error(errorCode))  
      break; 
    cout << output_msg << endl;
    PressEntertoContinue();
  } 
}

int main()
{
  Client client = Client(CONFIG_PATH);
  client.run();
}

