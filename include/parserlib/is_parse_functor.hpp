#ifndef PARSERLIB_IS_PARSE_FUNCTOR_HPP
#define PARSERLIB_IS_PARSE_FUNCTOR_HPP


#include <type_traits>
#include "parse_context.hpp"


namespace parserlib {


    /**
     * Checks if a functor has an `operator ()` which accepts a parse context.
     * @param T type of functor to check.
     */
    template <class T, class P = void> 
    struct is_parse_functor : std::false_type {
    };


    /**
     * Specialization for functor with `operator ()(parse_context<>&)`.
     * @param T type of functor to check.
     */
    template <class T> 
    struct is_parse_functor<T, std::void_t<decltype(std::declval<T>()(std::declval<parse_context<>&>()))>> : std::true_type {
    };


    /**
     * Helper variable for the `is_parse_functor` trait.
     */
    template <class T>
    constexpr bool is_parse_functor_v = is_parse_functor<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_PARSE_FUNCTOR_HPP
