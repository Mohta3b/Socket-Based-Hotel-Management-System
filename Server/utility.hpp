#pragma once
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;
using namespace std;

#define ONEDAYSECONDS 86400

json readJsonFile(string filePath)
{
  ifstream f(filePath);
  json data = json::parse(f);
  f.close();
  return data;
}

//  validaions
bool noSpace(string str)
{
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] == ' ')
      return false;
  }
  return true;
}
// username dont start with number
bool validUsername(string str)
{
  if (str[0] >= '0' && str[0] <= '9')
    return false;
  return true;
}
bool strongPassword(string str)
{
  if (str.size() < 4)
    return false;
  if (!isAlphaNum(str))
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

bool isNumber(string str)
{
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] < '0' || str[i] > '9')
      return false;
  }
  return true;
}

bool isAlpha(string str)
{
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z'))
      return false;
  }
  return true;
}

bool isAlphaNum(string str)
{
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < '0' || str[i] > '9'))
      return false;
  }
  return true;
}

// is phone number
bool isPhoneNumber(string str)
{
  if (str.size() != 11)
    return false;
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] < '0' || str[i] > '9')
      return false;
  }
  return true;
}

//  is address
bool isAddress(string str)
{
  for (int i = 0; i < str.size(); i++)
  {
    if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < '0' || str[i] > '9') && str[i] != '*')
      return false;
  }
  return true;
}

// tokenize
vector<string> tokenize(string msg, char delim = '\n')
{
  boost::trim(msg);
  // tokenize msg to username password
  stringstream ss(msg);
  string token;
  vector<string> tokens;
  while (getline(ss, token, delim))
  {
    tokens.push_back(token);
  }
  return tokens;
}

bool validRangeDate(string date1, string date2, string systemDate = "01-01-1800")
{
  // date1 = date1.substr(6, 4) + "-" + date1.substr(3, 2) + "-" + date1.substr(0, 2);
  // date2 = date2.substr(6, 4) + "-" + date2.substr(3, 2) + "-" + date2.substr(0, 2);
  // systemDate = systemDate.substr(6, 4) + "-" + systemDate.substr(3, 2) + "-" + systemDate.substr(0, 2);

  struct tm tm1 = {0}, tm2 = {0}, tm0 = {0};
  time_t time1, time2, time0;

  // Convert date strings to time_t
  if (strptime(systemDate.c_str(), "%d-%m-%Y", &tm0) == NULL)
    return false;
  time0 = mktime(&tm0);
  if (strptime(date1.c_str(), "%d-%m-%Y", &tm1) == NULL)
    return false;
  time1 = mktime(&tm1);
  if (strptime(date2.c_str(), "%d-%m-%Y", &tm2) == NULL)
    return false;
  time2 = mktime(&tm2);

  // Check if date1 is within range
  // we should use time1-time3 and is there are euqual the it is the edge case

  return difftime(time2, time1) >= 0 && difftime(time1, time0) >= 0;
}

time_t convertToDate(const std::string &dateString)
{ // time_t is an integer number of seconds from epoch
  std::tm tm = {};
  std::istringstream ss(dateString);
  ss >> std::get_time(&tm, "%d-%m-%Y");
  if (ss.fail())
  {
    std::cerr << "Error parsing date string " << dateString << std::endl;
    // dont we use log?
    return -1;
  }
  time_t t = mktime(&tm);
  if (t == -1)
  {
    std::cerr << "Error converting date string " << dateString << " to time_t" << std::endl;
    return -1;
  }
  return t;
}

int getNumOfDays(string date1, string date2)
{
  time_t t1 = convertToDate(date1);
  time_t t2 = convertToDate(date2);
  return (round((t2 - t1) / (ONEDAYSECONDS)) + 1);
}

bool isValidNewPassword(string newPassword, string oldPassword)
{
  if (!strongPassword(newPassword))
  {
    return false;
  }
  if (newPassword == oldPassword)
  {
    return false;
  }
  return true;
}

string convertSecondsToDate(time_t seconds)
{
  struct tm *timeinfo;
  char buffer[80];
  timeinfo = localtime(&seconds);
  strftime(buffer, 80, "%d-%m-%Y", timeinfo);
  return buffer;
}

void PressEntertoContinue()
{
  // stay in this fution until user type Enter
  cout << "Press Enter to continue...";
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  // clear screen
  system("clear");
}

string get_input()
{
  string input_msg;
  getline(cin, input_msg);
  boost::trim(input_msg);
  while (input_msg.find("  ") != std::string::npos)
    input_msg.erase(input_msg.find("  "), 1);
  return input_msg;
}

void convertAddress(string address)
{
  // replace white space with *
  for (int i = 0; i < address.size(); i++)
  {
    if (address[i] == ' ')
      address[i] = '*';
  }

}
