#include <string>

class User{
    public:
        User(int id, std::string clientName, std::string clientPassword, int clientBalance, std::string clientPhoneNumber, std::string clientAddress);
        int getId();
        std::string getClientName();
        std::string getClientPassword();
        int getClientBalance();
        std::string getClientPhoneNumber();
        std::string getClientAddress();
        void setId(int id);
        void setClientName(std::string clientName);
        void setClientPassword(std::string clientPassword);
        void setClientBalance(int clientBalance);
        void setClientPhoneNumber(std::string clientPhoneNumber);
        void setClientAddress(std::string clientAddress);
    private:
        int id;
        std::string clientName;
        std::string clientPassword;
        int clientBalance;
        std::string clientPhoneNumber;
        std::string clientAddress;
};

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
