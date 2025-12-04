#ifndef PARSERLIB_STRING_UTIL_HPP
#define PARSERLIB_STRING_UTIL_HPP


#include <string>
#include <type_traits>
#include <sstream>


namespace parserlib {


    inline const char hex_char(size_t index) {
        static const std::string chars = "0123456789ABCDEF";
        return chars[index & 15];
    }


    inline std::string hex(char16_t c) {
        std::stringstream stream;
        stream << hex_char(c >> 12);
        stream << hex_char(c >> 8);
        stream << hex_char(c >> 4);
        stream << hex_char(c);
        return stream.str();
    }


    inline std::string hex(char32_t c) {
        std::stringstream stream;
        stream << hex_char(c >> 28);
        stream << hex_char(c >> 24);
        stream << hex_char(c >> 20);
        stream << hex_char(c >> 16);
        stream << hex_char(c >> 12);
        stream << hex_char(c >> 8);
        stream << hex_char(c >> 4);
        stream << hex_char(c);
        return stream.str();
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_UTIL_HPP
