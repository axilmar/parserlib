#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <cstddef>
#include <type_traits>
#include <string>
#include <string_view>
#include "parser_wrapper_type.hpp"


namespace parserlib {


    template <class Parser> class zero_or_more_parser;
    template <class Parser> class one_or_more_parser;
    template <class Parser> class optional_parser;
    template <class Parser> class logical_and_parser;
    template <class Parser> class logical_not_parser;


    template <class Derived>
    class parser {
    public:
        auto operator *() const noexcept;

        auto operator +() const noexcept;

        auto operator -() const noexcept;

        auto operator &() const noexcept;

        auto operator !() const noexcept;
    };


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


#endif //PARSERLIB_PARSER_HPP
