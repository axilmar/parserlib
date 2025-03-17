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
    template <class Parser> class n_times_parser;
    template <class Parser> class optional_parser;
    template <class Parser> class logical_and_parser;
    template <class Parser> class logical_not_parser;


    /**
     * Base class for parsers.
     * It defines the unary operators used for the grammar.
     * @param Derived the derived parser class.
     */
    template <class Derived>
    class parser {
    public:
        /**
         * Creates a zero-or-more loop parser out of this parser.
         * @return a zero-or-more loop parser out of this parser.
         */
        auto operator *() const noexcept;

        /**
         * Creates an one-or-more loop parser out of this parser.
         * @return an one-or-more loop parser out of this parser.
         */
        auto operator +() const noexcept;

        /**
         * Creates an optional parser out of this parser.
         * @return an optional parser out of this parser.
         */
        auto operator -() const noexcept;

        /**
         * Creates a logical-and parser out of this parser.
         * @return a logical-and parser out of this parser.
         */
        auto operator &() const noexcept;

        /**
         * Creates a logical-not parser out of this parser.
         * @return a logical-not parser out of this parser.
         */
        auto operator !() const noexcept;

        /**
         * Creates a repetition of this parser for a specific count.
         * @param count number of times to repeat this parser.
         * @return the repetition parser.
         */
        auto operator [](std::size_t count) const noexcept;

        /**
         * Returns a pointer to derived class.
         * @return a pointer to derived class.
         */
        const Derived* pointer_to_derived() const noexcept {
            return static_cast<const Derived*>(this);
        }

        /**
         * Returns a pointer to derived class.
         * @return a pointer to derived class.
         */
        Derived* pointer_to_derived() noexcept {
            return static_cast<Derived*>(this);
        }
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


    template <class Derived>
    auto parser<Derived>::operator [](std::size_t count) const noexcept {
        return n_times_parser(get_parser_wrapper(*this), count);
    }


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
