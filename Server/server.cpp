// include needed libraries
#include <iostream>
#include <string>
#include <vector>
// json files
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
//#include "json.hpp"
// socket
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// include needed headers
#include "../Client/headers.hpp"
#include "admin.hpp"
#include "room.hpp"

void readConfigFile(std::string &hostName, int &portNumber){
    // read json file
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("config.json", pt);

    // get hostName and portNumber
    hostName = pt.get<std::string>("hostName");
    portNumber = pt.get<int>("commandChannelPort");
}

int setupServer(int port)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed\n");
        printf("error in binding\n");
        return -1;
    }

    listen(server_fd, 4);

    return server_fd;
}

void createServer(){
    // read hostName and portNumber from config.json
    std::string hostName;
    int portNumber;
    readConfigFile(hostName, portNumber);

    // setup server
    int server_fd = setupServer(portNumber);
    printf("server is running on port %d\n", server_fd);
}

void readUsersFile(std::vector<User> &users, std::vector<Admin> &admins)
{
    // read json file
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("users.json", pt);

    // get users
    for (boost::property_tree::ptree::value_type &v : pt.get_child("users"))
    {
        // get user id
        int id = v.second.get<int>("id");

        // get user name
        std::string userName = v.second.get<std::string>("userName");

        // get user password
        std::string userPassword = v.second.get<std::string>("userPassword");

        // get admin
        bool isAdmin = v.second.get<bool>("isAdmin");

        // get balance
        int balance = v.second.get<int>("balance");

        // get phone number
        std::string phoneNumber = v.second.get<std::string>("phoneNumber");

        // get address
        std::string address = v.second.get<std::string>("address");

        // create user
        User user(id, userName, userPassword, balance, phoneNumber, address);

        // add user to users vector
        users.push_back(user);
    }

    // get admins
    for (boost::property_tree::ptree::value_type &v : pt.get_child("admins"))
    {
        // get admin id
        int id = v.second.get<int>("id");

        // get admin name
        std::string admin_name = v.second.get<std::string>("adminName");

        // get admin password
        std::string admin_password = v.second.get<std::string>("adminPassword");

        // get admin
        bool isAdmin = v.second.get<bool>("isAdmin");

        // create admin
        Admin admin(id, admin_name, admin_password);

        // add admin to admins vector
        admins.push_back(admin);
    }


}


void readRoomsFile(std::vector<Room> &rooms)
{
    // read json file
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("RoomsInfo.json", pt);

    // get rooms
    for (boost::property_tree::ptree::value_type &v : pt.get_child("rooms"))
    {
        // get room number
        int room_number = v.second.get<int>("roomNumber");

        // get room price
        float price = v.second.get<float>("price");

        // get room capacity
        int capacity = v.second.get<int>("capacity");

        // get room max capacity
        int max_capacity = v.second.get<int>("maxCapacity");

        // get room status
        bool status = v.second.get<bool>("status");

        // get reserved users
        std::vector<bookedClient> reserved_users;
        for (boost::property_tree::ptree::value_type &v : v.second.get_child("reservedUsers"))
        {
            // get user id
            int user_id = v.second.get<int>("userId");

            // get number of reserved beds by this user
            int reserved_beds = v.second.get<int>("reservedBeds");

            // get check in date
            std::string check_in_date = v.second.get<std::string>("checkInDate");

            // get check out date
            std::string check_out_date = v.second.get<std::string>("checkOutDate");

            // create reserved user
            bookedClient reserved_user(user_id, reserved_beds, check_in_date, check_out_date);

            // add reserved user to reserved users vector
            reserved_users.push_back(reserved_user);
        }

        // create room
        Room room(room_number,status, price, max_capacity, capacity, reserved_users);
        
        // add room to rooms vector
        rooms.push_back(room);
    }
}

int main()
{
    createServer();
    
    std::vector<Admin> admins;
    std::vector<User> users;
    
    readUsersFile(users, admins);

    std::vector<Room> rooms;
    readRoomsFile(rooms);

    return 0;
}
