#ifndef PARSERLIB_FUNCTION_PARSER_HPP
#define PARSERLIB_FUNCTION_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Function>
    class function_parser : public parser<function_parser<Function>> {
    public:
        function_parser(const Function& function) noexcept
            : m_function(function)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position()) {
                return m_function(context);
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
    function_parser<Function> function(const Function& function) noexcept {
        return function;
    }


} //namespace parserlib


#endif //PARSERLIB_FUNCTION_PARSER_HPP
