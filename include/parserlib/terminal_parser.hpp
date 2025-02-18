#ifndef PARSERLIB_TERMINAL_PARSER_HPP
#define PARSERLIB_TERMINAL_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Terminal> 
    class terminal_parser : public parser<terminal_parser<Terminal>> {
    public:
        terminal_parser(const Terminal& terminal) noexcept
            : m_terminal(terminal)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const noexcept {
            if (context.is_valid_parse_position() && ParseContext::compare_tokens(*context.parse_position(), m_terminal) == 0) {
                context.increment_parse_position();
                return true;
            }
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            return parse(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            return false;
        }

    private:
        const Terminal m_terminal;
    };


    template <class Terminal>
    terminal_parser<Terminal> terminal(const Terminal& terminal) noexcept {
        return terminal;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_PARSER_HPP
