#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <ctime>
#include <chrono>

// be careful about reading from json file cause of extra fields!!!!!!

using json = nlohmann::json;




class bookedClient{
    public:
        bookedClient(int id, int numberofBeds, std::string reserveDate, std::string checkoutDate , int roomNumber);
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

    private:
        int id;
        int numberofBeds;
        std::string reserveDate;
        std::string checkoutDate;
        int roomNumber;
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

class Room{
    public:
        Room(int room_number, bool status, float price, int maxCapacity, int currentCapacity , std::vector<bookedClient*> bookedClients);
        Room(){};
        int getNumber();
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
        // add client
        void addClient(bookedClient* client);
        std::string getRoomInfo();
        std::string getBookedClientsString();
        bool isRoomAvailable(std::string startDate, std::string endDate, int bedsNumber);
    private:
        int room_number;
        bool status;
        float price;
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


Room::Room(int number, bool status, float price, int maxCapacity, int currentCapacity, std::vector<bookedClient*> bookedClients){
    this->room_number = number;
    this->status = status;
    this->price = price;
    this->maxCapacity = maxCapacity;
    this->currentCapacity = currentCapacity;
    this->bookedClients = bookedClients;
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
}

void Room::setCurrentCapacity(int currentCapacity){
    this->currentCapacity = currentCapacity;
}

void Room::setBookedClients(std::vector<bookedClient*> bookedClients){
    this->bookedClients = bookedClients;
}

void Room::addClient(bookedClient* client) {
    this->bookedClients.push_back(client);
}

std::string Room::getBookedClientsString(){
    std::string msg = "";
    for (int i = 0; i < bookedClients.size(); i++)
    {
      // cur room BookedClients;
      msg += "User ID: " + to_string(bookedClients[i]->getId()) 
      + "\nNumber of Reserved Beds: " + to_string(bookedClients[i]->getNumberofBeds())
      + "\nReservervation Date: " + bookedClients[i]->getReserveDate()
      + "\nCheck Out Date: " + bookedClients[i]->getCheckoutDate() + "\n";
    }
    return msg;
}

std::string Room::getRoomInfo(){
  std::string msg="";
  msg += "Room Number: " + to_string(room_number) + "\nRoom Price: " +
  to_string(price) + "\nRoom Current Residents: " +
  to_string(currentCapacity) + "\nRoom Status: " + this->getStatusString() + 
  " ( " + to_string(this->getMaxCapacity() - this->getCurrentCapacity()) +
    " / " + to_string(this->getMaxCapacity()) + " )\n" ;
  return msg;
}


bool Room::isRoomAvailable(std::string startDate, std::string endDate, int bedsNumber)
{
    time_t checkInTime = convertToDate(startDate);
    time_t checkOutTime = convertToDate(endDate);
    
    int maxBedsBooked = 0;
    for (time_t date = checkInTime; date <= checkOutTime; date += 86400)
    {
        int totalBedsBooked = 0;
        for (const auto& reservation : bookedClients) 
        {
            time_t startReserveDate = convertToDate(reservation->getReserveDate());
            time_t  endReserveDate = convertToDate(reservation->getCheckoutDate());
            if (date >= startReserveDate && date < endReserveDate) 
            {
                totalBedsBooked += reservation->getNumberofBeds();
            }
        }
        if (totalBedsBooked + bedsNumber > maxCapacity) 
        {
            return false;
        }
    }
    return true;

    
}