// #include <iostream> 
// #include <vector>
// #include <string>
// #include <fstream>
// #include <regex>

// using namespace std;

// struct SP {
//   int a=0;
// };

// int main () {
//   string date_pattern = "(^(((0[1-9]|1[0-9]|2[0-8])\
// [-](0[1-9]|1[012]))|((29|30|31)\
// [-](0[13578]|1[02]))|((29|30)\
// [-](0[4,6,9]|11)))\
// [-](19|[2-9][0-9])\\d\\d$)|(^29[-]02\
// [-](19|[2-9][0-9])(00|04|08|12|16|20|24|28|32|36|40|44|48|52|56|60|64|68|72|76|80|84|88|92|96)$)";
//   cout << date_pattern << endl;
//   regex dateRegex(date_pattern);
//   string date = "29-02-2020";
//   string false_date = "35-02-2020";
//   if (regex_match(date, dateRegex)) {
//     cout << "Date is valid" << endl;
//   } else {
//     cout << "Date is invalid" << endl;
//   }

//   if (regex_match(false_date, dateRegex)) {
//     cout << "Date is valid" << endl;
//   } else {
//     cout << "Date is invalid" << endl;
//   }
// }

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include "./Server/Logs.hpp"
struct Reservation {
    int numberOfBeds;
    std::string checkInDate;
    std::string checkOutDate;
};

bool canReserveRoom(int numberOfBeds, const std::string& checkInDate, const std::string& checkOutDate, const std::vector<Reservation>& reservations) {
    int capacity = 0;
    for (const Reservation& reservation : reservations) {
       std::string reservationCheckInDate = reservation.checkInDate.substr(0, 2) + "-" + reservation.checkInDate.substr(3, 2) + "-" + reservation.checkInDate.substr(6, 4);
      std::string reservationCheckOutDate = reservation.checkOutDate.substr(0, 2) + "-" + reservation.checkOutDate.substr(3, 2) + "-" + reservation.checkOutDate.substr(6, 4);
        
        if (checkOutDate <= reservation.checkInDate || checkInDate >= reservation.checkOutDate) {
            // The current reservation does not overlap with the requested dates, so we don't need to consider its capacity
            continue;
        }
        capacity += reservation.numberOfBeds;
    }
    return capacity + numberOfBeds <= 10;
}
std::vector<std::string> tokenize(std::string msg, char delim='\n') {
  boost::trim(msg);
  //tokenize msg to username password
  std::stringstream ss(msg);
  std::string token;
  std::vector<std::string> tokens;
  while (getline(ss, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}
int main() {
    // // const int MAX_CAPACITY = 10;
    // // std::vector<Reservation> reservations = {
    // //     {4, "10-03-2023", "15-03-2023"},
    // //     {3, "17-03-2023", "20-03-2023"}
    // // };
    // std::string idate1 = "10-04-2023";
    // std::string idate2 = "17-03-2023";

    // idate1 = idate1.substr(6, 4) + "-" + idate1.substr(3, 2) + "-" + idate1.substr(0, 2);
    // idate2 = idate2.substr(6, 4) + "-" + idate2.substr(3, 2) + "-" + idate2.substr(0, 2);
    // std::cout << idate1 << " " << idate2 << std::endl;
    // //std::string systemDate = isystemDate.substr(0, 2) + "-" + isystemDate.substr(3, 2) + "-" + isystemDate.substr(6, 4);
    // if (idate1 > idate2) {
    //   std::cout << "date1" << std::endl;
    // }
    // else {
    //   std::cout << "date2222" << std::endl;
    // }
    // return 0;
    // std::string ss,ss1,ss2;
    // std::stringstream ss3;
    // // read input from cin until enter is pressed then exit the while loop
    // std::getline(std::cin, ss);
    // while(ss.find("  ") != std::string::npos) {
    //     ss.erase(ss.find("  "), 1);
    // }
    // std::cout << ss << std::endl;
    
    // std::vector<std::string> args = tokenize(ss, ' ');
    // //std::cout << args[0] << std::endl;
    // std::cout << args.size() << std::endl;

    // std::cin >> ss >> ss1 >> ss2;
    // std::cout << ss <<ss1 <<ss2 << std::endl;

    const char *server_log_directory = "./Server/Log";
    struct stat sb;
    if (!(stat(server_log_directory, &sb) == 0 && S_ISDIR(sb.st_mode))) // directory does not exists
        boost::filesystem::create_directories(server_log_directory);

}
