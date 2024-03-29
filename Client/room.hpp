#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <ctime>
#include <chrono>
#include "../Server/utility.hpp"
// be careful about reading from json file cause of extra fields!!!!!!

using json = nlohmann::json;

#define ONEDAYSECONDS 86400
#define FULL 1
#define EMPTY 0


class bookedClient{
    public:
        bookedClient(int id, int numberofBeds, std::string reserveDate,
         std::string checkoutDate , int roomNumber);
        bookedClient(){};
        int getId();
        int getNumberofBeds();
        std::string getReserveDate();
        std::string getCheckoutDate();
        int getRoomNumber();
        void setId(int id);
        void setNumberofBeds(int numberofBeds);
        void setReserveDate(std::string reserveDate);
        void setCheckoutDate(std::string checkoutDate);
        void setRoomNumber(int roomNumber);
        int getReserveDuration();
    private:
        int id; // unique client id
        int numberofBeds;
        std::string reserveDate;
        std::string checkoutDate;
        int roomNumber; // unique
        friend void to_json(json& j, const bookedClient& bc);
        friend void from_json(const json& j, bookedClient& bc);      
};

void to_json(json& j, const bookedClient& bc) {
    j = json{ {"id", bc.id}, {"numOfBeds", bc.numberofBeds},
     {"reserveDate", bc.reserveDate}, {"checkoutDate", bc.checkoutDate} };
}

void from_json(const json& j, bookedClient& bc) {
    j.at("id").get_to(bc.id);
    j.at("numOfBeds").get_to(bc.numberofBeds);
    j.at("reserveDate").get_to(bc.reserveDate);
    j.at("checkoutDate").get_to(bc.checkoutDate);
}

bookedClient::bookedClient(int id, int numberofBeds, std::string reserveDate, std::string checkoutDate, int roomNumber){
    this->id = id;
    this->numberofBeds = numberofBeds;
    this->reserveDate = reserveDate;
    this->checkoutDate = checkoutDate;
    this->roomNumber = roomNumber;
}

int bookedClient::getId(){
    return this->id;
}

int bookedClient::getNumberofBeds(){
    return this->numberofBeds;
}

std::string bookedClient::getReserveDate(){
    return this->reserveDate;
}

std::string bookedClient::getCheckoutDate(){
    return this->checkoutDate;
}

int bookedClient::getRoomNumber(){
    return this->roomNumber;
}

void bookedClient::setId(int id){
    this->id = id;
}

void bookedClient::setNumberofBeds(int numberofBeds){
    this->numberofBeds = numberofBeds;
}

void bookedClient::setReserveDate(std::string reserveDate){
    this->reserveDate = reserveDate;
}

void bookedClient::setCheckoutDate(std::string checkoutDate){
    this->checkoutDate = checkoutDate;
}

void bookedClient::setRoomNumber(int roomNumber){
    this->roomNumber = roomNumber;
}

int bookedClient::getReserveDuration() {
    return getNumOfDays( this->reserveDate, this->checkoutDate);
}


////////////////////////////// Room Class //////////////////////////////
class Room{
    public:
        Room(int number, bool status, float price, int maxCapacity,
         int currentCapacity, std::vector<bookedClient*> bookedClients);
        Room(int number, int maxCapacity , int price);
        Room(){};
        int getNumber(); // unique
        bool getStatus();
        std::string getStatusString();
        float getPrice();
        int getMaxCapacity();
        int getCurrentCapacity();
        std::vector<bookedClient*> getBookedClients();
        void setStatus(bool status);
        void setPrice(float price);
        void setMaxCapacity(int maxCapacity);
        void setCurrentCapacity(int currentCapacity);
        void setBookedClients(std::vector<bookedClient*> bookedClients);
        void setBookedClient();
        // add client
        void addBookedClient(bookedClient* client);
        std::string getRoomInfo();
        std::string getBookedClientsString();
        bool isRoomAvailable(std::string startDate, std::string endDate,
         int bedsNumber, int clientId);
        void update();
        void updateStatus(int numOfBeds);
        int findClientbyId(int userId);
        void updateRoomCapacity(int numOfBeds);
        void deleteBookedClient(int id);
        void deleteAllBookedClientsbyDate(std::string date);
        bool hasReservation();
        void editBookedClient(int bookedClientindex, int numOfBeds,
          std::string startDate, std::string endDate);
        void passDay(std::string date);
    private:
        int room_number;
        int status;
        int price;
        int maxCapacity;
        int currentCapacity;
        std::vector<bookedClient*> bookedClients;
        friend void to_json(json& j, const Room& r);
        friend void from_json(const json& j, Room& r);      
};

void to_json(json& j, const Room& r) {
    j = json{ {"number", r.room_number}, {"status", r.status},
     {"price", r.price}, {"maxCapacity", r.maxCapacity},
      {"capacity", r.currentCapacity} };
    // add booked clients
    // creat empty vector as json["users"]
    j["users"] = json::array();
    for(auto& element : r.bookedClients){
        json j2;
        to_json(j2, *element);
        j["users"].push_back(j2);
    }
}

void from_json(const json& j, Room& r) {
    j.at("number").get_to(r.room_number);
    j.at("status").get_to(r.status);
    j.at("price").get_to(r.price);
    j.at("maxCapacity").get_to(r.maxCapacity);
    j.at("capacity").get_to(r.currentCapacity);
    // add booked clients from json file to vector
    // vector<bookedClient> bc;
    // j.at("users").get_to(bc);
    
    for(auto& element : j.at("users")){
        bookedClient& bc = *(new bookedClient());
        from_json(element, bc);
        // set room number
        bc.setRoomNumber(r.room_number);
        r.bookedClients.push_back(&bc);
    }
    
}


Room::Room(int number, bool status, float price, int maxCapacity,
 int currentCapacity, std::vector<bookedClient*> bookedClients){
    this->room_number = number;
    this->status = status;
    this->price = price;
    this->maxCapacity = maxCapacity;
    this->currentCapacity = currentCapacity;
    this->bookedClients = bookedClients;
}

Room::Room(int number, int maxCapacity, int price){
    this->room_number = number;
    this->status = EMPTY;
    this->price = price;
    this->maxCapacity = maxCapacity;
    this->currentCapacity = EMPTY;
}
int Room::getNumber(){
    return this->room_number;
}

bool Room::getStatus(){
    return this->status;
}

std::string Room::getStatusString(){
    if (this->status == 0)
        return "empty";
    else 
        return "full";

}

float Room::getPrice(){
    return this->price;
}

int Room::getMaxCapacity(){
    return this->maxCapacity;
}

int Room::getCurrentCapacity(){
    return this->currentCapacity;
}

std::vector<bookedClient*> Room::getBookedClients(){
    return this->bookedClients;
}

void Room::setStatus(bool status){
    this->status = status;
}

void Room::setPrice(float price){
    this->price = price;
}

void Room::setMaxCapacity(int maxCapacity){
    this->maxCapacity = maxCapacity;
    updateStatus(0);
}

void Room::setCurrentCapacity(int currentCapacity){
    this->currentCapacity = currentCapacity;
}

void Room::setBookedClients(std::vector<bookedClient*> bookedClients){
    this->bookedClients = bookedClients;
}

void Room::addBookedClient(bookedClient* client) {
    this->bookedClients.push_back(client);
}

std::string Room::getBookedClientsString(){
    std::string msg = "";
    for (int i = 0; i < bookedClients.size(); i++)
    {
      // cur room BookedClients;
      msg += "User ID: " + std::to_string(bookedClients[i]->getId()) 
      + "\nNumber of Reserved Beds: " + std::to_string(bookedClients[i]->getNumberofBeds())
      + "\nReservervation Date: " + bookedClients[i]->getReserveDate()
      + "\nCheck Out Date: " + bookedClients[i]->getCheckoutDate() + "\n";
    }
    return msg;
}

std::string Room::getRoomInfo(){
  std::string msg="";
  msg += "Room Number: " + std::to_string(room_number) + "\nRoom Price: " +
  std::to_string(price) + "\nRoom Current Residents: " +
  std::to_string(currentCapacity) + "\nRoom Status: " + this->getStatusString() + 
  " ( " + std::to_string(this->getMaxCapacity() - this->getCurrentCapacity()) +
    " / " + std::to_string(this->getMaxCapacity()) + " )\n" ;
  return msg;
}


bool Room::isRoomAvailable(std::string startDate, std::string endDate,
 int bedsNumber, int clientId)
{
    // is repeated is wrong
    int index = findClientbyId(clientId);
    if( index != -1 && bookedClients[index]->getNumberofBeds() != 0)
        return false;
    time_t checkInTime = convertToDate(startDate);
    time_t checkOutTime = convertToDate(endDate);
    int size = round(difftime(checkOutTime,checkInTime) / ONEDAYSECONDS);
    size++;
    vector<int> dates_res_num(size,bedsNumber);
    for (const auto& reservation : bookedClients) 
    {
        time_t startReserveDate = convertToDate(reservation->getReserveDate());
        // find index by regarding checkInTime as base
        int index1 = round(difftime(startReserveDate,checkInTime) / ONEDAYSECONDS);
        time_t  endReserveDate = convertToDate(reservation->getCheckoutDate());
        int index2 = round(difftime(endReserveDate,checkInTime) / ONEDAYSECONDS);

        int start,end;
        start = max(index1,0);
        end =  min(index2,size-1);
        for(int i = start; i <= end; i++)
        {
            dates_res_num[i] += reservation->getNumberofBeds();
            if (dates_res_num[i] > maxCapacity) {
                return false;
            }
        }
    }
    return true;    
}

void Room::update() {

}
void Room::updateStatus(int numOfBeds) {
    if (currentCapacity + numOfBeds == maxCapacity) {
        status = FULL;
    }
    else {
        status = EMPTY;
    }
}

int Room::findClientbyId(int id) {
    for (int i = 0; i < bookedClients.size(); i++)
    {
        if (bookedClients[i]->getId() == id) {
            return i;
        }
    }
    return -1;
}

void Room::updateRoomCapacity(int numOfBeds) {
    currentCapacity += numOfBeds;
    updateStatus(0);
}

void Room::deleteBookedClient(int id) {
    int index = findClientbyId(id);
    if (index != -1) {
        delete bookedClients[index]; 
        bookedClients.erase(bookedClients.begin() + index);
    }
}

void Room::deleteAllBookedClientsbyDate(std::string date) 
{
    date = date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2);
    for (int i = 0; i < bookedClients.size(); i++)
    {
        std::string checkInDate = bookedClients[i]->getReserveDate();
        checkInDate = checkInDate.substr(6, 4) + "-" + checkInDate.substr(3, 2) 
        + "-" + checkInDate.substr(0, 2);
        if (checkInDate <= date) 
        {
            delete bookedClients[i];
            bookedClients.erase(bookedClients.begin() + i);
            i--;
        }
    }
}

bool Room::hasReservation()
{
    return bookedClients.size() > 0;
}


void Room::editBookedClient(int bookedClientindex, int numOfBeds,
  std::string startDate, std::string endDate) {
  bookedClient& client = *(bookedClients[bookedClientindex]);
  client.setNumberofBeds(numOfBeds);
  client.setReserveDate(startDate);
  client.setCheckoutDate(endDate);
}
       
void Room::passDay(std::string date) {
  date = date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2);
  for (int i = 0; i < bookedClients.size(); i++)
  {
    std::string checkOutDate = bookedClients[i]->getCheckoutDate();
    checkOutDate = checkOutDate.substr(6, 4) + "-" +
     checkOutDate.substr(3, 2) + "-" + checkOutDate.substr(0, 2);
    if (checkOutDate < date)
    {
      updateRoomCapacity(-bookedClients[i]->getNumberofBeds());
      delete bookedClients[i];
      bookedClients.erase(bookedClients.begin() + i);
      i--;
    }
  }
}


