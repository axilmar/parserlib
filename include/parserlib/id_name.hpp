#ifndef PARSERLIB_ID_NAME_HPP
#define PARSERLIB_ID_NAME_HPP


#include <sstream>


namespace parserlib {


    template <class T> 
    const std::string id_name(const T& id) {
        std::stringstream stream;
        stream << id;
        return stream.str();
    }


} //namespace parserlib


#endif //PARSERLIB_ID_NAME_HPP
