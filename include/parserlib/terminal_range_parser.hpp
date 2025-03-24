#ifndef PARSERLIB_TERMINAL_RANGE_PARSER_HPP
#define PARSERLIB_TERMINAL_RANGE_PARSER_HPP


#include <cassert>
#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that checks if a token is within a range of terminal values.
     * @param Terminal terminal value.
     */
    template <class Terminal>
    class terminal_range_parser : public parser<terminal_range_parser<Terminal>> {
    public:
        /**
         * The constructor.
         * @param min min value of the range.
         * @param max max value of the range.
         */
        terminal_range_parser(const Terminal& min, const Terminal& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        /**
         * Parses a single token against the terminal value range.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position()) {
                const auto token = *context.parse_position();
                if (ParseContext::compare_tokens(token, m_min) >= 0 && ParseContext::compare_tokens(token, m_max) <= 0) {
                    context.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        /**
         * Parses a single token against the terminal value range.
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

    private:
        const Terminal m_min;
        const Terminal m_max;
    };


    /**
     * Helper function for creating a terminal range parser.
     * @param min min value of the range.
     * @param max max value of the range.
     * @return the terminal range parser.
     */
    template <class Terminal>
    terminal_range_parser<Terminal> range(const Terminal& min, const Terminal& max) {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_RANGE_PARSER_HPP
