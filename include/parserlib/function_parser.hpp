#ifndef PARSERLIB_FUNCTION_PARSER_HPP
#define PARSERLIB_FUNCTION_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that invokes a function for parsing.
     * @param Function the function to invoke for parsing; signature: `bool(ParseContext&)`.
     */
    template <class Function>
    class function_parser : public parser<function_parser<Function>> {
    public:
        /**
         * The constructor.
         * @param function the function to invoke for parsing.
         */
        function_parser(const Function& function) noexcept
            : m_function(function)
        {
        }

        /**
         * Invokes the internal function for parsing.
         * @param context parse context.
         * @return if the parse position is valid, then whatever the function returns, otherwise false.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position()) {
                return m_function(context);
            }
            return false;
        }

        /**
         * Invokes the internal function for parsing.
         * @param context parse context.
         * @return if the parse position is valid, then whatever the function returns, otherwise false.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        /**
         * It does not invoke the internal function for parsing,
         * as this parser state is for left-recursive rules in the left-recursion-continuation phase.
         * @return always false.
         */
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
