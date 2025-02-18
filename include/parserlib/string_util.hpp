#ifndef PARSERLIB_STRING_UTIL_HPP
#define PARSERLIB_STRING_UTIL_HPP


#include <cstddef>


namespace parserlib {


    template <class T>
    std::size_t null_terminated_string_length(const T* str) noexcept {
        std::size_t length = 0;
        for (; *str != '\0'; ++str, ++length) {
        }
        return length;
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_UTIL_HPP
