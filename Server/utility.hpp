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
