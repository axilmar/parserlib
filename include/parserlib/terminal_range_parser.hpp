#ifndef PARSERLIB_TERMINAL_RANGE_PARSER_HPP
#define PARSERLIB_TERMINAL_RANGE_PARSER_HPP


#include <cassert>
#include "parser.hpp"


namespace parserlib {


    template <class Terminal>
    class terminal_range_parser : public parser<terminal_range_parser<Terminal>> {
    public:
        terminal_range_parser(const Terminal& min, const Terminal& max) noexcept
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

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

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return false;
        }

    private:
        const Terminal m_min;
        const Terminal m_max;
    };


    template <class Terminal>
    terminal_range_parser<Terminal> terminal(const Terminal& min, const Terminal& max) noexcept {
        return { min, max };
    }


    template <class Terminal>
    terminal_range_parser<Terminal> range(const Terminal& min, const Terminal& max) noexcept {
        return { min, max };
    }


    template <class Terminal>
    terminal_range_parser<Terminal> within(const Terminal& min, const Terminal& max) noexcept {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_RANGE_PARSER_HPP
