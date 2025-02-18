#ifndef PARSERLIB_PARSER_UNARY_OPERATORS_HPP
#define PARSERLIB_PARSER_UNARY_OPERATORS_HPP


#include "zero_or_more_parser.hpp"
#include "one_or_more_parser.hpp"
#include "optional_parser.hpp"
#include "logical_and_parser.hpp"
#include "logical_not_parser.hpp"


namespace parserlib {


template <class Derived>
auto parser<Derived>::operator *() const noexcept {
    return zero_or_more_parser(get_parser_wrapper(*this));
}


template <class Derived>
auto parser<Derived>::operator +() const noexcept {
    return one_or_more_parser(get_parser_wrapper(*this));
}


template <class Derived>
auto parser<Derived>::operator -() const noexcept {
    return optional_parser(get_parser_wrapper(*this));
}


template <class Derived>
auto parser<Derived>::operator &() const noexcept {
    return logical_and_parser(get_parser_wrapper(*this));
}


template <class Derived>
auto parser<Derived>::operator !() const noexcept {
    return logical_not_parser(get_parser_wrapper(*this));
}


} //namespace parserlib


#endif //PARSERLIB_PARSER_UNARY_OPERATORS_HPP
