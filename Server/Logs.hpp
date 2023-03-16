#pragma once
// use ifndef
#ifndef LOGS_HPP
#define LOGS_HPP
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

#ifndef BOOST_ALL_DYN_LINK
#   define BOOST_ALL_DYN_LINK
#endif
#include <boost/filesystem.hpp>

#define SYSTEM 0
#define USER 1
#define ADMIN 2

void logEvent(int type, int log_id, std::string detail = "", int user_id = 0);




#endif