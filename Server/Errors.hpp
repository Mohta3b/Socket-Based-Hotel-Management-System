#include <iostream>



struct error_info{
    int code;
    char *description;
};

std::map<int, struct error_info> ErrorMap = {
    {101, {1, (char *)"The desired room was not found!"}},
    {102, {2, (char *)"Your reservation was not found!"}},
    {104, {3, (char *)"Successfully added!"}},
    {105, {4, (char *)"Successfuly modified!"}},
    {106, {5, (char *)"Successfully deleted!"}},
    {108, {7, (char *)"Your account balance is not enough!"}},
    {109, {8, (char *)"The room capacity is full!"}},
    {110, {9, (char *)"Successfully done!"}},
    {111, {10, (char *)"This room already exists!"}},
    {201, {11, (char *)"User logged out successfully!"}},
    {230, {12, (char *)"User logged in!"}},
    {231, {13, (char *)"User successfully signed up!"}},
    {311, {14, (char *)"User Signed up. Enter your password, purse, phone and address."}},
    {312, {15, (char *)"Information was changed successfully!"}},
    {401, {16, (char *)"Invalid value!"}},
    {403, {17, (char *)"Access denied!"}},
    {412, {18, (char *)"Invalid capacity value!"}},
    {413, {19, (char *)"Successfully Leaving!"}},
    {430, {20, (char *)"Invalid username or password!"}},
    {451, {21, (char *)"User already existed!"}},
    {503, {22, (char *)"Bad sequence of commands!"}},
    {200, {23, (char *)"OK!SUCCESS!"}}
};

std::string get_error(int error_code){
    return ErrorMap[error_code].description;
}



// int main(){
    
//     std::cout << get_error(413) << std::endl;
//     //std::cout << error.description << std::endl;
//     return 0;
// }
