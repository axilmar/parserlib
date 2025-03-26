#ifndef PARSERLIB_BOOLEAN_PARSER_HPP
#define PARSERLIB_BOOLEAN_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A boolean parser.
     * It allows either true or false as a predicate.
     */
    class boolean_parser : public parser<boolean_parser> {
    public:
        /**
         * The constructor.
         * @param value the boolean value.
         */
        boolean_parser(bool value) 
            : m_value(value)
        {
        }

        /**
         * The parse function.
         * @param context the parse context.
         * @return the internal boolean value.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return m_value;
        }

        /**
         * The parse function for the left-recursion-start state.
         * @param context the parse context.
         * @return the internal boolean value.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return m_value;
        }

        /**
         * The parse function for the left-recursion-continuation state.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return the internal boolean value.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return m_value;
        }

    private:
        const bool m_value;
    };


    //implementation
    inline boolean_parser get_parser_wrapper(bool value) {
        return { value };
    }


} //namespace parserlib


#endif //PARSERLIB_BOOLEAN_PARSER_HPP
