#ifndef PARSERLIB_TERMINAL_STRING_PARSER_HPP
#define PARSERLIB_TERMINAL_STRING_PARSER_HPP


#include <string>
#include <string_view>
#include <vector>
#include "parser.hpp"
#include "string_util.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal string.
     * @param Terminal type of terminal parser.
     */
    template <class Terminal>
    class terminal_string_parser : public parser<terminal_string_parser<Terminal>> {
    public:
        /**
         * Constructor from range.
         * @param begin start of range.
         * @param end end of range.
         */
        template <class Iterator>
        terminal_string_parser(const Iterator& begin, const Iterator& end)
            : m_terminal(begin, end)
        {
        }

        /**
         * Constructor from null-terminated string.
         * @param str null-terminated string.
         */
        terminal_string_parser(const Terminal* str)
            : terminal_string_parser(str, str + null_terminated_string_length(str))
        {
        }

        /**
         * Constructor from std::basic_string.
         * @param str string.
         */
        template <class Traits, class Alloc>
        terminal_string_parser(const std::basic_string<Terminal, Traits, Alloc>& str)
            : m_terminal(str.begin(), str.end())
        {
        }

        /**
         * Constructor from std::basic_string_view.
         * @param str string view.
         */
        template <class Traits>
        terminal_string_parser(const std::basic_string_view<Terminal, Traits>& str)
            : m_terminal(str.begin(), str.end())
        {
        }

        /**
         * Parses the input against the string.
         * If found equal, using the comparator specified in parse definitions of the context,
         * then the context parse position is advanced as many positions as the string length.
         * Otherwise, the context is not affected.
         * @param context the parse context.
         * @return true if parsing succeeds, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            auto pos = context.parse_position();
            auto it = m_terminal.begin();
            for (;;) {
                if (it == m_terminal.end()) {
                    break;
                }
                if (pos == context.end_position()) {
                    return false;
                }
                if (ParseContext::compare_tokens(*pos, *it) != 0) {
                    return false;
                }
                ++pos;
                ++it;
            }
            context.increment_parse_position(m_terminal.size());
            return true;
        }

        /**
         * Same as `parse(context)`.
         * @param context the parse context.
         * @return true if parsing succeeds, false on failure.
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
        std::vector<Terminal> m_terminal;
    };


    /**
     * Creates a terminal string parser from a range of characters.
     * @param begin start of range.
     * @param end end of range.
     * @return a terminal string parser.
     */
    template <class Iterator>
    terminal_string_parser<typename Iterator::value_type> terminal(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }


    /**
     * Creates a terminal string parser from a null-terminated string.
     * @param str null-terminated string.
     * @return a terminal string parser.
     */
    template <class Terminal>
    terminal_string_parser<Terminal> terminal(const Terminal* str) {
        return str;
    }


    /**
     * Creates a terminal string parser from an std::basic_string.
     * @param str string.
     * @return a terminal string parser.
     */
    template <class Terminal, class Traits, class Alloc>
    terminal_string_parser<Terminal> terminal(const std::basic_string<Terminal, Traits, Alloc>& str) {
        return str;
    }


    /**
     * Creates a terminal string parser from an std::basic_string_view.
     * @param str string view.
     * @return a terminal string parser.
     */
    template <class Terminal, class Traits>
    terminal_string_parser<Terminal> terminal(const std::basic_string_view<Terminal, Traits>& str) {
        return str;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_STRING_PARSER_HPP
