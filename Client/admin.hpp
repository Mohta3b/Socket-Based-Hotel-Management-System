#include <string>

class Admin{
    public:
        Admin(int id, std::string adminName, std::string adminPassword);
        int getId();
        std::string getAdminName();
        std::string getAdminPassword();
        void setId(int id);
        void setAdminName(std::string adminName);
        void setAdminPassword(std::string adminPassword);
    private:
        int id;
        std::string adminName;
        std::string adminPassword;
};


Admin::Admin(int id, std::string adminName, std::string adminPassword){
    this->id = id;
    this->adminName = adminName;
    this->adminPassword = adminPassword;
}

int Admin::getId(){
    return this->id;
}

std::string Admin::getAdminName(){
    return this->adminName;
}

std::string Admin::getAdminPassword(){
    return this->adminPassword;
}

void Admin::setId(int id){
    this->id = id;
}

void Admin::setAdminName(std::string adminName){
    this->adminName = adminName;
}

void Admin::setAdminPassword(std::string adminPassword){
    this->adminPassword = adminPassword;
}
