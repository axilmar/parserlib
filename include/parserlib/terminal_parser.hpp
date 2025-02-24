#ifndef PARSERLIB_TERMINAL_PARSER_HPP
#define PARSERLIB_TERMINAL_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal value.
     * @param Terminal the terminal value.
     */
    template <class Terminal> 
    class terminal_parser : public parser<terminal_parser<Terminal>> {
    public:
        /**
         * The constructor.
         * @param terminal the terminal value.
         */
        terminal_parser(const Terminal& terminal) noexcept
            : m_terminal(terminal)
        {
        }

        /**
         * Parses a single token against the terminal value.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position() && ParseContext::compare_tokens(*context.parse_position(), m_terminal) == 0) {
                context.increment_parse_position();
                return true;
            }
            return false;
        }

        /**
         * Parses a single token against the terminal value.
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
        const Terminal m_terminal;
    };


    /**
     * Helper function for creating a terminal parser.
     * @param terminal terminal value to create a terminal parser parser for.
     * @return the terminal parser.
     */
    template <class Terminal>
    terminal_parser<Terminal> terminal(const Terminal& terminal) noexcept {
        return terminal;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_PARSER_HPP
