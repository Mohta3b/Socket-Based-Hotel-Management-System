#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;




class Admin{
    public:
        Admin(int id, std::string adminName, std::string adminPassword);
        Admin(){};
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

        friend void to_json(json& j, const Admin& a);
        friend void from_json(const json& j, Admin& a);
};

void to_json(json& j, const Admin& a) {
    j = json{ {"id", a.id}, {"username", a.adminName},
     {"password", a.adminPassword} };
}

void from_json(const json& j, Admin& a) {
    j.at("id").get_to(a.id);
    j.at("username").get_to(a.adminName);
    j.at("adminPassword").get_to(a.adminPassword);
}

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
