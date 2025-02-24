#ifndef PARSERLIB_LOGICAL_AND_PARSER_HPP
#define PARSERLIB_LOGICAL_AND_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that tests another parser as a predicate.
     * After parsing, the context's state is restored to the state before the call.
     * @param Parser the parser to test.
     */
    template <class Parser>
    class logical_and_parser : public parser<logical_and_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to test.
         */
        logical_and_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Invokes the internal parser, then restores the context to the state before the call.
         * @param context parse context.
         * @return the result of the internal parser.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = m_parser.parse(context);
            context.set_state(state);
            return result;
        }

        /**
         * Invokes the internal parser, in the context of left recursion start, 
         * then restores the context to the state before the call.
         * @param context parse context.
         * @return the result of the internal parser.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = m_parser.parse_left_recursion_start(context);
            context.set_state(state);
            return result;
        }

        /**
         * Invokes the internal parser, in the context of left recursion continuation,
         * then restores the context to the state before the call.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return the result of the internal parser.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            const auto state = context.get_state();
            const bool result = m_parser.parse_left_recursion_continuation(context, state);
            context.set_state(state);
            return result;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSER_HPP
