#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;
using namespace std;

class person {
    std::string name;
    std::string address;
    int age;
    friend void to_json(json& j, const person& p);
    friend void from_json(const json& j, person& p);
    bool operator== (person p1);

    public:
      person(std::string name, std::string address, int age){this->name = name;this->address = address;this->age =age;};
      std::string get_address();
  
};

std::string person::get_address(){
  return this->address;
}

bool person::operator== (person p1) {
  if (this->name == p1.name)
    return true;
  else return false;
}
void to_json(json& j, const person& p) {
    j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
}

void from_json(const json& j, person& p) {
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
}

int main() {
  std::ifstream f("config.json");
  json data = json::parse(f);
  data["tarr"]["1"] = { 1, 0, 2 };
  string j_str = data.dump();
  json j_obs = json::parse(j_str);
  j_obs["rooms"] =     { 
      {"currency", "USD"},
       {"value", 42.99}
    };

  std::cout << data["commandChannelPort"] << endl;
  std::cout << j_obs.dump() << endl;
  vector<int> nums =  j_obs.at("tarr").at("1");
  std::cout << nums[1]<< endl;


// store a string in a JSON value
json j_string = "this is a string";

// // retrieve the string value
// auto cpp_string = j_string.get<std::string>();
// // retrieve the string value (alternative when a variable already exists)
// std::string cpp_string2;
// j_string.get_to(cpp_string2);

// // retrieve the serialized value (explicit JSON serialization)
// std::string serialized_string = j_string.dump();

// // output of original string
// std::cout << cpp_string << " == " << cpp_string2 << " == " << j_string.get<std::string>() << '\n';
// // output of serialized value
// std::cout << j_string << " == " << serialized_string << std::endl;
// cout << j_string << endl;
// std::vector<std::uint8_t> v = {'f', 'a', 'l', 's', 'e'};
// json j = json::parse(v);
// cout << j << endl;
// 
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


person p=person("Ned Flanders", "744 Evergreen Terrace", 60);

// conversion: person -> json
json j = {p,p};

std::cout << j[0]["name"] << std::endl;
string ss = j[0]["name"];
std::cout << ss << '\n';
std::cout << p.get_address() << std::endl;
// {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}

// conversion: json -> person

// that's it
// assert(p == p2);
}
