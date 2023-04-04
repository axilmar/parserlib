#ifndef TESTPL_ERROR_HPP
#define TESTPL_ERROR_HPP


#include <string>
#include "Position.hpp"


//error
struct Error {
    Position position;
    std::string message;
    std::string content;
};


#endif //TESTPL_ERROR_HPP
