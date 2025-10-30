#ifndef PARSERLIB_IS_PARSE_FUNCTOR_HPP
#define PARSERLIB_IS_PARSE_FUNCTOR_HPP


#include "parse_context.hpp"


namespace parserlib {


    /**
     * Checks if a functor has an `operator ()` which accepts a parse context.
     * @param T type of functor to check.
     */
    template <class T, class P = void> struct is_parse_functor {
        static constexpr bool value = false;
    };


    /**
     * Specialization for functor with `operator ()(parse_context<>&)`.
     * @param T type of functor to check.
     */
    template <class T> struct is_parse_functor<T, decltype(std::declval<T>()(std::declval<parse_context<>&>()), void())> {
        static constexpr bool value = true;
    };


    /**
     * Helper variable for the `is_parse_functor` trait.
     */
    template <class T>
    constexpr bool is_parse_functor_v = is_parse_functor<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_PARSE_FUNCTOR_HPP
