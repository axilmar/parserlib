#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <cstddef>
#include <type_traits>
#include <string>
#include <string_view>
#include "parser_wrapper_type.hpp"


namespace parserlib {


    template <class Derived>
    class parser {
    public:
        using parser_type = Derived;

        auto operator *() const noexcept;

        auto operator +() const noexcept;

        auto operator -() const noexcept;

        auto operator &() const noexcept;

        auto operator !() const noexcept;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
