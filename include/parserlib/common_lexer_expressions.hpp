#ifndef PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
#define PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP


#include <cctype>
#include "terminal_range_parser.hpp"
#include "function_parser.hpp"


namespace parserlib {


    /**
     * Creates a digit parser.
     * Digits from '0' to '9' are considered.
     * @return a range parser for digits.
     */
    inline auto digit() noexcept {
        return range('0', '9');
    }


    /**
     * Creates a lowercase letter parser, using the function std::islower.
     * @return a lowercase letter parser.
     */
    inline auto lowercase_letter() noexcept {
        return function([](auto& context) {
            return std::islower(static_cast<int>(*context.parse_position()));
            });
    }


    /**
     * Creates an uppercase letter parser, using the function std::islower.
     * @return an uppercase letter parser.
     */
    inline auto uppercase_letter() noexcept {
        return function([](auto& context) {
            return std::isupper(static_cast<int>(*context.parse_position()));
            });
    }


    /**
     * Creates a letter parser, using the function std::isalpha.
     * @return a letter parser.
     */
    inline auto letter() noexcept {
        return function([](auto& context) {
            return std::isalpha(static_cast<int>(*context.parse_position()));
            });
    }


    /**
     * Creates a whitespace parser, using the function std::isspace.
     * @return a whitespace parser.
     */
    inline auto whitespace() noexcept {
        return function([](auto& context) {
            return std::isspace(static_cast<int>(*context.parse_position()));
            });
    }


} //namespace parserlib


#endif //PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
