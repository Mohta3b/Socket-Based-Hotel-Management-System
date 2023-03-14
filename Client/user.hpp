#include <ctime>
#include <string>
#include <nlohmann/json.hpp>
#include "./room.hpp"


#include "../Server/utility.hpp"
using json = nlohmann::json;



class User{
    public:
        User(int id, std::string clientName, std::string clientPassword,
         int clientBalance, std::string clientPhoneNumber, std::string clientAddress);
        User(){};
        int getId();
        std::string getClientName();
        std::string getClientPassword();
        int getClientBalance();
        std::string getClientPhoneNumber();
        std::string getClientAddress();
        std::string getReservedRooms();
        void setId(int id);
        void setClientName(std::string clientName);
        void setClientPassword(std::string clientPassword);
        void setClientBalance(int clientBalance);
        void setClientPhoneNumber(std::string clientPhoneNumber);
        void setClientAddress(std::string clientAddress);
        void addReservedRoom(bookedClient* bookedClient);
        std::string showFutureReservations(std::string date);
        int deleteFutureReservation(int roomNum, int bedToCancle, std::string date)
    private:
        int id;
        std::string clientName;
        std::string clientPassword;
        int clientBalance;
        std::string clientPhoneNumber;
        std::string clientAddress;
        std::vector<bookedClient*> reservedRooms;
        friend void to_json(json& j, const User& u);
        friend void from_json(const json& j, User& u);
};

void to_json(json& j, const User& u) {
    j = json{ {"id", u.id}, {"username", u.clientName},
     {"password", u.clientPassword}, {"balance", u.clientBalance},
     {"phoneNumber", u.clientPhoneNumber}, {"address", u.clientAddress} };
}

void from_json(const json& j, User& u) {
    j.at("id").get_to(u.id);
    j.at("username").get_to(u.clientName);
    j.at("password").get_to(u.clientPassword);
    j.at("balance").get_to(u.clientBalance);
    j.at("phoneNumber").get_to(u.clientPhoneNumber);
    j.at("address").get_to(u.clientAddress);
}

User::User(int id, std::string clientName, std::string clientPassword, int clientBalance, std::string clientPhoneNumber, std::string clientAddress){
    this->id = id;
    this->clientName = clientName;
    this->clientPassword = clientPassword;
    this->clientBalance = clientBalance;
    this->clientPhoneNumber = clientPhoneNumber;
    this->clientAddress = clientAddress;
}

int User::getId(){
    return this->id;
}

std::string User::getClientName(){
    return this->clientName;
}

std::string User::getClientPassword(){
    return this->clientPassword;
}

int User::getClientBalance(){
    return this->clientBalance;
}

std::string User::getClientPhoneNumber(){
    return this->clientPhoneNumber;
}

std::string User::getClientAddress(){
    return this->clientAddress;
}


std::string User::getReservedRooms(){
    std::string reservedRooms = "";
    for(int i = 0; i < this->reservedRooms.size(); i++){
        reservedRooms += to_string(this->reservedRooms[i]->getRoomNumber()) + " | " 
        + to_string(this->reservedRooms[i]->getNumberofBeds()) + " | "
        + this->reservedRooms[i]->getReserveDate() + " | "
        + this->reservedRooms[i]->getCheckoutDate() + "\n";
    }
    return reservedRooms;
}

void User::setId(int id){
    this->id = id;
}

void User::setClientName(std::string clientName){
    this->clientName = clientName;
}   

void User::setClientPassword(std::string clientPassword){
    this->clientPassword = clientPassword;
}

void User::setClientBalance(int clientBalance){
    this->clientBalance = clientBalance;
}

void User::setClientPhoneNumber(std::string clientPhoneNumber){
    this->clientPhoneNumber = clientPhoneNumber;
}

void User::setClientAddress(std::string clientAddress){
    this->clientAddress = clientAddress;
}

void User::addReservedRoom(bookedClient* bookedClient){
    this->reservedRooms.push_back(bookedClient);
}

std::string User::showFutureReservations(string date){
    time_t today= convertToDate(date);
    std::string msg = "";
    for(int i = 0; i < this->reservedRooms.size(); i++){
        bookedClient cur_reserv = *(reservedRooms[i]);
        if(difftime(convertToDate(cur_reserv.getReserveDate()), today) > 0) {
          // it is futurre reserve
          // show msg: roomNumber | numberofBeds | reserveDate | checkoutDate
          msg += std::to_string(cur_reserv.getRoomNumber()) + " | "
          + std::to_string(cur_reserv.getNumberofBeds()) + " | "
          + cur_reserv.getReserveDate() + " | "
          + cur_reserv.getCheckoutDate() + "\n";
        }
    }
    return msg;
}

int User::deleteFutureReservation(int roomNum, int bedToCancle, std::string date) {
    time_t today= convertToDate(date);
    int error_num = 101;
    for(int i = 0; i < this->reservedRooms.size(); i++){
        bookedClient& cur_reserv = *(reservedRooms[i]);
        if(difftime(convertToDate(cur_reserv.getReserveDate()), today) > 0) {
          // it is futurre reserve
          if (cur_reserv.getRoomNumber() == roomNum) {
            error_num = 102;
            // delete this reservation
            if(cur_reserv.getNumberofBeds() >= bedToCancle) {
              cur_reserv.setNumberofBeds(cur_reserv.getNumberofBeds() - bedToCancle);
              return 110;
            }            
          }
        }
    }
    return error_num;   
}
