#ifndef PARSERLIB_TERMINAL_FUNCTION_PARSER_HPP
#define PARSERLIB_TERMINAL_FUNCTION_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Function>
    class terminal_function_parser : public parser<terminal_function_parser<Function>> {
    public:
        terminal_function_parser(const Function& function) noexcept
            : m_function(function)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position() && m_function(*context.parse_position())) {
                context.increment_parse_position();
                return true;
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
        const Function m_function;
    };


    template <class Function>
    terminal_function_parser<Function> terminal_function(const Function& function) noexcept {
        return function;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_FUNCTION_PARSER_HPP
