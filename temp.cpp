#include <iostream> 
#include <vector>
#include <string>
#include <fstream>
#include <regex>

using namespace std;

struct SP {
  int a=0;
};

int main () {
  string date_pattern = "(^(((0[1-9]|1[0-9]|2[0-8])\
[-](0[1-9]|1[012]))|((29|30|31)\
[-](0[13578]|1[02]))|((29|30)\
[-](0[4,6,9]|11)))\
[-](19|[2-9][0-9])\\d\\d$)|(^29[-]02\
[-](19|[2-9][0-9])(00|04|08|12|16|20|24|28|32|36|40|44|48|52|56|60|64|68|72|76|80|84|88|92|96)$)";
  cout << date_pattern << endl;
  regex dateRegex(date_pattern);
  string date = "29-02-2020";
  string false_date = "35-02-2020";
  if (regex_match(date, dateRegex)) {
    cout << "Date is valid" << endl;
  } else {
    cout << "Date is invalid" << endl;
  }

  if (regex_match(false_date, dateRegex)) {
    cout << "Date is valid" << endl;
  } else {
    cout << "Date is invalid" << endl;
  }
}

