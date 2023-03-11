#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;




class bookedClient{
    public:
        bookedClient(int id, int numberofBeds, std::string reserveDate, std::string checkoutDate);
        bookedClient(){};
        int getId();
        int getNumberofBeds();
        std::string getReserveDate();
        std::string getCheckoutDate();
        int getNumberOfDays();
        void setId(int id);
        void setNumberofBeds(int numberofBeds);
        void setReserveDate(std::string reserveDate);
        void setCheckoutDate(std::string checkoutDate); 
    
    private:
        int id;
        int numberofBeds;
        std::string reserveDate;
        std::string checkoutDate;
        int numberOfDays;
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

bookedClient::bookedClient(int id, int numberofBeds, std::string reserveDate, std::string checkoutDate){
    this->id = id;
    this->numberofBeds = numberofBeds;
    this->reserveDate = reserveDate;
    this->checkoutDate = checkoutDate;
    this->numberOfDays = 0;
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

int bookedClient::getNumberOfDays(){
    return this->numberOfDays;
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


class Room{
    public:
        Room(int room_number, bool status, float price, int maxCapacity, int currentCapacity , std::vector<bookedClient> bookedClients);
        Room(){};
        int getNumber();
        bool getStatus();
        float getPrice();
        int getMaxCapacity();
        int getCurrentCapacity();
        std::vector<bookedClient> getBookedClients();
        void setStatus(bool status);
        void setPrice(float price);
        void setMaxCapacity(int maxCapacity);
        void setCurrentCapacity(int currentCapacity);
        void setBookedClients(std::vector<bookedClient> bookedClients);
    private:
        int room_number;
        bool status;
        float price;
        int maxCapacity;
        int currentCapacity;
        std::vector<bookedClient> bookedClients;
        friend void to_json(json& j, const Room& r);
        friend void from_json(const json& j, Room& r);      
};

void to_json(json& j, const Room& r) {
    j = json{ {"number", r.room_number}, {"status", r.status},
     {"price", r.price}, {"maxCapacity", r.maxCapacity},
      {"capacity", r.currentCapacity}, {"users", r.bookedClients} };
}

void from_json(const json& j, Room& r) {
    j.at("number").get_to(r.room_number);
    j.at("status").get_to(r.status);
    j.at("price").get_to(r.price);
    j.at("maxCapacity").get_to(r.maxCapacity);
    j.at("capacity").get_to(r.currentCapacity);
    j.at("users").get_to(r.bookedClients);
    
    // for(auto& element : j.at("users")){
    //     bookedClient bc;
    //     from_json(element, bc);
    //     r.bookedClients.push_back(bc);
    // }
}


Room::Room(int number, bool status, float price, int maxCapacity, int currentCapacity, std::vector<bookedClient> bookedClients){
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

float Room::getPrice(){
    return this->price;
}

int Room::getMaxCapacity(){
    return this->maxCapacity;
}

int Room::getCurrentCapacity(){
    return this->currentCapacity;
}

std::vector<bookedClient> Room::getBookedClients(){
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

void Room::setBookedClients(std::vector<bookedClient> bookedClients){
    this->bookedClients = bookedClients;
}




