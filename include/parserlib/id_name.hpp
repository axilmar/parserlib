#ifndef PARSERLIB_ID_NAME_HPP
#define PARSERLIB_ID_NAME_HPP


#include <type_traits>


namespace parserlib {


    template <class T, class P = void> struct id_name {
        static const auto get(const T& value) {
            if constexpr (std::is_enum_v<T>) {
                return static_cast<int>(value);
            }
            else {
                return value;
            }
        }
    };


    template <class T> struct id_name<T, std::void_t<decltype(get_id_name(std::declval<T>()))>> {
        static auto get(const T& value) {
            return get_id_name(value);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ID_NAME_HPP
