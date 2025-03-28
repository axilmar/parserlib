#ifndef PARSERLIB_STRING_UTIL_HPP
#define PARSERLIB_STRING_UTIL_HPP


#include <cstddef>


namespace parserlib {


    /**
     * Counts the number of characters in the given string, until '\0' is found.
     * Provided because c++ lacks a uniform way to count 8-bit, 16-bit and 32-bit characters.
     * @param str null-terminated string.
     * @return length of string.
     */
    template <class T>
    std::size_t null_terminated_string_length(const T* str) {
        std::size_t length = 0;
        for (; *str != '\0'; ++str, ++length) {
        }
        return length;
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_UTIL_HPP
