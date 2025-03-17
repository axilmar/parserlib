#ifndef PARSERLIB_TERMINAL_ANY_PARSER_HPP
#define PARSERLIB_TERMINAL_ANY_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that parses any terminal value.
     */
    class terminal_any_parser : public parser<terminal_any_parser> {
    public:
        /**
         * Parses any single token.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position()) {
                context.increment_parse_position();
                return true;
            }
            return false;
        }

        /**
         * Parses any single token.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        /**
         * Does nothing, since in the pase left recursion continuation phase,
         * the parse position should not be advanced.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return false.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return false;
        }
    };


    /**
     * Helper function for creating a terminal any parser.
     * @return the terminal any parser.
     */
    inline terminal_any_parser any() noexcept {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_ANY_PARSER_HPP
