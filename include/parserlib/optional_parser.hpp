#ifndef PARSERLIB_OPTIONAL_PARSER_HPP
#define PARSERLIB_OPTIONAL_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that invokes an internal parser, always returning true.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class optional_parser : public parser<optional_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        optional_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Invokes the internal parser.
         * @param context parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            m_parser.parse(context);
            return true;
        }

        /**
         * Invokes the internal parser,
         * in the context of left recursion start.
         * @param context parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            m_parser.parse_left_recursion_start(context);
            return true;
        }

        /**
         * Invokes the internal parser,
         * in the context of left recursion continuation.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return always true.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            m_parser.parse_left_recursion_continuation(context, context.get_state());
            return true;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSER_HPP
