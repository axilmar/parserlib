#ifndef PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
#define PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP


#include <cctype>
#include "terminal_range_parser.hpp"
#include "function_parser.hpp"
#include "terminal_string_parser.hpp"


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


    template <class Start, class Char, class End>
    auto block_comment(const Start& start, const Char& ch, const End& end) noexcept {
        return start >> *(ch - end) >> end;
    }


    template <class Start, class Char>
    auto line_comment(const Start& start, const Char& ch) noexcept {
        return block_comment(start, ch, newline('\n') | end());
    }


    inline auto string_escaped_char() noexcept {
        return terminal("\0") | "\n" | "\r" | "\t" | "\v" | "\\" | "\\\"" | "\\\'";
    }


    inline auto hex_prefix() noexcept {
        return one_of('x', 'X');
    }


    inline auto hex_2_digits() noexcept {
        return hex_prefix() || hex_char()[2];
    }


    inline auto string_char() noexcept {
        return string_escaped_char() | (any() - string_escaped_char);
    }


} //namespace parserlib


#endif //PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
