#ifndef PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
#define PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP


#include <cctype>
#include "parsers.hpp"


namespace parserlib {


    /**
     * Creates a digit parser.
     * Digits from '0' to '9' are considered.
     * @return a range parser for digits.
     */
    inline auto digit() {
        return range('0', '9');
    }


    /**
     * Creates a lowercase letter parser, using the function std::islower.
     * @return a lowercase letter parser.
     */
    inline auto lowercase_letter() {
        return function([](auto& context) {
            if (std::islower(static_cast<int>(*context.parse_position()))) {
                context.increment_parse_position();
                return true;
            }
            return false;
        });
    }


    /**
     * Creates an uppercase letter parser, using the function std::islower.
     * @return an uppercase letter parser.
     */
    inline auto uppercase_letter() {
        return function([](auto& context) {
            if (std::isupper(static_cast<int>(*context.parse_position()))) {
                context.increment_parse_position();
                return true;
            }
            return false;
        });
    }


    /**
     * Creates a letter parser, using the function std::isalpha.
     * @return a letter parser.
     */
    inline auto letter() {
        return function([](auto& context) {
            if (std::isalpha(static_cast<int>(*context.parse_position()))) {
                context.increment_parse_position();
                return true;
            }
            return false;
        });
    }


    /**
     * Creates a whitespace parser, using the function std::isspace.
     * @return a whitespace parser.
     */
    inline auto whitespace() {
        return function([](auto& context) {
            if (std::isspace(static_cast<int>(*context.parse_position()))) {
                context.increment_parse_position();
                return true;
            }
            return false;
        });
    }


    /**
     * Creates a block comment.
     * @param Start block comment start.
     * @param Char block comment character.
     * @param End block comment end.
     * @return a grammar that parses a block comment.
     */
    template <class Start, class Char, class End>
    auto block_comment(const Start& start, const Char& ch, const End& end) {
        return start >> *(ch - end) >> end;
    }


    /**
     * Creates a line comment.
     * The end of a line comment is newline('\n') or the end of input.
     * @param Start line comment start.
     * @param Char block comment character.
     * @return a grammar that parses a line comment.
     */
    template <class Start, class Char>
    auto line_comment(const Start& start, const Char& ch) {
        return block_comment(start, ch, newline('\n') | end());
    }


} //namespace parserlib


#endif //PARSERLIB_COMMON_LEXER_EXPRESSIONS_HPP
