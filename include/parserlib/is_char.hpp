#ifndef PARSERLIB_IS_CHAR_HPP
#define PARSERLIB_IS_CHAR_HPP


#include <cuchar>


namespace parserlib {


    template <class T> struct is_char {
        static constexpr bool value = false;
    };


    template <> struct is_char<unsigned char> {
        static constexpr bool value = true;
    };


    template <> struct is_char<signed char> {
        static constexpr bool value = true;
    };


    template <> struct is_char<char> {
        static constexpr bool value = true;
    };


    template <> struct is_char<wchar_t> {
        static constexpr bool value = true;
    };


    #ifdef char8_t
    template <> struct is_char<char8_t> {
        static constexpr bool value = true;
    };
    #endif


    template <> struct is_char<char16_t> {
        static constexpr bool value = true;
    };


    template <> struct is_char<char32_t> {
        static constexpr bool value = true;
    };


    template <class T>
    constexpr bool is_char_v = is_char<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_CHAR_HPP
