#ifndef PARSERLIB_IS_RANGE_HPP
#define PARSERLIB_IS_RANGE_HPP


#include <type_traits>


namespace parserlib {


    /**
     * Trait for checking if a type is a range (i.e. if it has `begin()` and `end()` methods).
     * @param T type to check.
     */
    template <class T, class P = void> struct is_range : std::false_type {
    };


    /**
     * Implementation of the `is_range` trait for types that are ranges.
     * @param T type to check.
     */
    template <class T> struct is_range<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {
    };


    /**
     * Shortcut to the `is_range::value` member.
     * @param T type to check.
     */
    template <class T>
    constexpr bool is_range_v = is_range<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_RANGE_HPP
