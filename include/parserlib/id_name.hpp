#ifndef PARSERLIB_ID_NAME_HPP
#define PARSERLIB_ID_NAME_HPP


#include <type_traits>


namespace parserlib {


    /**
     * Implementation of id name for types that don't have a global function `get_id_name`.
     * @param T the type to get the id name of.
     */
    template <class T, class P = void> struct id_name {
        /**
         * Returns the id name for a value.
         * @param value the value to get the id name of.
         * @return the name representation of the value; enums are cast to `int`.
         */
        static const auto get(const T& value) {
            if constexpr (std::is_enum_v<T>) {
                return static_cast<int>(value);
            }
            else {
                return value;
            }
        }
    };


    /**
     * Implementation of id name for types that have a global function `get_id_name`.
     * @param T the type to get the id name of.
     */
    template <class T> struct id_name<T, std::void_t<decltype(get_id_name(std::declval<T>()))>> {
        /**
         * Returns the id name for a value.
         * @param value the value to get the id name of.
         * @return the name representation of the value.
         */
        static auto get(const T& value) {
            return get_id_name(value);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ID_NAME_HPP
