#include <iostream>
#include "admin.hpp"
#include "user.hpp"
#include "room.hpp"

#define EMPTY 0
#define FULL 1


const char *viewUserInfo()
{
}

const char *viewRoomsInfo(bool show_full_rooms = true){
    std::string rooms_info;
    for(auto room : rooms){
        if (room.status == FULL && show_full_rooms)


    }
}