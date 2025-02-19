#ifndef PARSERLIB_TERMINAL_STRING_PARSER_HPP
#define PARSERLIB_TERMINAL_STRING_PARSER_HPP


#include <string>
#include <string_view>
#include <vector>
#include "parser.hpp"
#include "string_util.hpp"


namespace parserlib {


    template <class Terminal>
    class terminal_string_parser : public parser<terminal_string_parser<Terminal>> {
    public:
        terminal_string_parser(const Terminal* begin, const Terminal* end) noexcept
            : m_terminal(begin, end)
        {
        }

        terminal_string_parser(const Terminal* nullTerminatedString) noexcept
            : terminal_string_parser(nullTerminatedString, nullTerminatedString + null_terminated_string_length(nullTerminatedString))
        {
        }

        template <class Iterator>
        terminal_string_parser(const Iterator& begin, const Iterator& end) noexcept
            : m_terminal(begin, end)
        {
        }

        template <class Traits, class Alloc>
        terminal_string_parser(const std::basic_string<Terminal, Traits, Alloc>& str) noexcept
            : m_terminal(str.begin(), str.end())
        {
        }

        template <class Traits>
        terminal_string_parser(const std::basic_string_view<Terminal, Traits>& str) noexcept
            : m_terminal(str.begin(), str.end())
        {
        }

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

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return false;
        }

    private:
        std::vector<Terminal> m_terminal;
    };


    template <class Terminal>
    terminal_string_parser<Terminal> terminal(const Terminal* begin, const Terminal* end) noexcept {
        return { begin, end };
    }


    template <class Terminal>
    terminal_string_parser<Terminal> terminal(const Terminal* nullTerminatedString) noexcept {
        return nullTerminatedString;
    }


    template <class Iterator>
    terminal_string_parser<typename Iterator::value_type> terminal(const Iterator& begin, const Iterator& end) noexcept {
        return { begin, end };
    }


    template <class Terminal, class Traits, class Alloc>
    terminal_string_parser<Terminal> terminal(const std::basic_string<Terminal, Traits, Alloc>& str) noexcept {
        return str;
    }


    template <class Terminal, class Traits>
    terminal_string_parser<Terminal> terminal(const std::basic_string_view<Terminal, Traits>& str) noexcept {
        return str;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_STRING_PARSER_HPP
