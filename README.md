# Hotel Management System
A terminal command based hotel management system. This is the first computer assignment of Computer Networks course.

- [Features](#hotel-management-system)
    - [Server](#server) 
        - [Setup Server](#setup-server)
        - [Client Connection](#client-connection) 
        - [Shut Down Server](#shut-down-server) 
    - [Client](#client)
        - [System](#system)
        - [User](#user)
        - [Admin](#admin)
     - [Log](#log)
        - [Server Log](#server-log)
        - [User Log](#user-log)
- [How to use?](#how-to-use)


## Server

### Setup Server
By running `./server.out`, the system asks you to enter the start date of the server.\
The correct date format is "***dd-mm-yyyy***" and if the format is not entered correctly, the system asks you to enter the start date again.\
If the date format entered is correct, the system displays the number of admins, users, and rooms that have been initialized.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226169099-ffcabeed-b040-4069-af5b-3487812037d1.png">
</p>


### Client Connection
For each client that connects to the server, a message is displayed in the server's terminal.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226170416-e9df19b9-f54c-4324-9435-3185c0e79c13.png">
</p>

If the client disconnects from the server (close its terminal), a message is displayed in the server terminal saying "rcv go 0 from: fd" where fd is the client's file descriptor.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226170950-0e002705-966b-4a1b-899a-c618d21c9c12.png">
</p>

### Shut Down Server
By typing `exit`, the server will shut down and all changes made to hotel information (users, admins, rooms, etc.) will be saved in json files for possible reuse.
In this case, if the clients who are still connected to the server enter a command, the following message will be displayed for them and the program will terminate.
```text
ERROR reading from socket: Success
```
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226171569-a2328f7c-dfc2-42a2-969e-307cb27f1074.png">
</p>


## Client
The client has 3 types of commands:
1. [System Command](#system)
2. [User Command](#user)
3. [Admin Command](#admin)

### System
At the beginning of the program, this menu is displayed, which consists of the following two commands:

- Signup\
By entering `0` in command line, a user can sign up by enering his/her information.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226173956-d573fbbf-7843-44c6-b36a-0773083eb1a7.png">
</p>

- Login\
By entering `1` in command line, a user can login to his/her account.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226174095-df829bc1-7360-46aa-b354-7a4c86049598.png">
</p>

***NOTE:*** 
You can cancel the **Sign up** and **Login** operations by typing `quit` at any stage. It returns the user to the [system command menu](#system).


### User
When the client is successfully logged in and its type is user, this menu is displayed with the following commands.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226174563-ad4c42d6-4695-42bc-8d52-d6ee09da447a.png">
</p>

***NOTE:*** 
Run the commands as given in the parentheses after them.\
If there are no parentheses for a command, just enter the command number.


### Admin
When the client is successfully logged in and its type is admin, this menu is displayed with the following commands.
<p align="center">
<img src="https://user-images.githubusercontent.com/86144768/226174721-12600933-8288-49ee-932e-cbc25a6832d0.png">
</p>

***NOTE:*** 
Run the commands as given in the parentheses after them.\
If there are no parentheses for a command, just enter the command number.


## Log
### Server Log
It contains all the events with their dates.
The `log.txt` file is located in `./Server/Log/`.
### User Log
For each user, its events will be logged in a folder named based on their unique ids.\
If the user type is *user*, the `log.txt` file is located in `./Client/Logs/Users/user_id/`, where *user_id* is the unique identifier of the user.
If the user type is *admin*, the file `log.txt` is located in `./Client/Logs/Admins/user_id/`, where *user_id* is the unique identifier of the administrator.

## How to use?
Follow these steps to run this repo in linux terminal:
- navigate to the path of the project
- open the terminal
- run the following command to build the program:
```text
make
```
- run the following command to create a server:
```text
./server.out
```
- in any other terminal, run the following command to create a client:
```text
./client.out
```

***NOTE 1:***
You can change the socket configuration by navigating to `./Server/` and editing the `config.json` file.

***NOTE 2:***
You can change the user configuration or add administrators or users by going to `./Server/` and editing the `UsersInfo.json` file.

***NOTE 3:***
You can change the hotel rooms configuration by going to `./Server/` and editing the `RoomsInfo.json` file.

