#include <iostream>
#include <map>
#define SYSTEM 0
#define USER 1
#define ADMIN 2

void logEvent(int type, int log_id, std::string detail = "", int user_id = 0);
std::map<int,std::string> logs = {
    {0,"set server"},
    {1, "read json files"},
    {2, "client connected to server"},
    {3, "user signed up"},
    {4, "user logged out"},
    {5 , "failed attempt to sign in"},
    {6, "user signed in"},
    {7, "request: view user info"},
    {8, "request: view rooms info"},
    {9, "request: booking"},
    {10, "request: canceling"},
    {11, "request: edit user info"},
    {12, "request: leaving the room"},
    {13, "unknown command"},
    {14, "request: view all users info"},
    {15, "request: pass a day"},
    {16, "request: remove guests from room"},
    {17, "request: add room"},
    {18, "request: modify room"},
    {19, "request: remove room"},
    {20, "client disconnected"},
    {21, "server shut down"},
    {22,"setup server failed!"}
};