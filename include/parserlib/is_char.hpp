#ifndef PARSERLIB_IS_CHAR_HPP
#define PARSERLIB_IS_CHAR_HPP


#include <type_traits>


namespace parserlib {


    template <class T> class is_char : public std::false_type {};
    template <> class is_char<char> : public std::true_type {};
    template <> class is_char<signed char> : public std::true_type {};
    template <> class is_char<unsigned char> : public std::true_type {};
    #ifdef char8_t
    template <> class is_char<char8_t> : public std::true_type {};
    #endif
    template <> class is_char<char16_t> : public std::true_type {};
    template <> class is_char<char32_t> : public std::true_type {};
    template <> class is_char<wchar_t> : public std::true_type {};


    template <class T> constexpr bool is_char_v = is_char<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_CHAR_HPP
