#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
using json = nlohmann::json;
using namespace std;

json readJsonFile(string filePath) {
  ifstream f(filePath);
  json data = json::parse(f);
  f.close();
  return data;
}

//  validaions
bool noSpace(string str) {
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] == ' ')
      return false;
  }
  return true;
}
// username dont start with number
bool validUsername(string str) {
  if (str[0] >= '0' && str[0] <= '9')
    return false;
  return true;
}
bool strongPassword(string str) {
  if (str.size() < 4)
    return false;
  bool hasNum = false;
  bool hasAlpha = false;
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] >= '0' && str[i] <= '9')
      hasNum = true;
    if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
      hasAlpha = true;
    if (hasNum && hasAlpha)
      break;
  }
  return hasNum && hasAlpha;
}

bool isNumber(string str) {
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] < '0' || str[i] > '9')
      return false;
  }
  return true;
}

bool isAlpha(string str) {
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z'))
      return false;
  }
  return true;
}

bool isAlphaNum(string str) {
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < '0' || str[i] > '9'))
      return false;
  }
  return true;
}

// is phone number
bool isPhoneNumber(string str) {
  if (str.size() !=11)
    return false;
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] < '0' || str[i] > '9')
      return false;
  }
  return true;
}

//  is address
bool isAddress(string str) {
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < '0' || str[i] > '9') && str[i] != ' ' && str[i] != ',')
      return false;
  }
  return true;
}