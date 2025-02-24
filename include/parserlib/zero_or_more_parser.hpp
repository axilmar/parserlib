#ifndef PARSERLIB_ZERO_OR_MORE_PARSER_HPP
#define PARSERLIB_ZERO_OR_MORE_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that repeats the internal parser zero or more times.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class zero_or_more_parser : public parser<zero_or_more_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        zero_or_more_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Parses the internal parser in a loop.
         * @param context the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            while (true) {
                const bool result = m_parser.parse(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        /**
         * Parses the internal parser in a loop,
         * in the context of left recursion start.
         * @param context the parse context.
         * @return always true.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            while (true) {
                const bool result = m_parser.parse_left_recursion_start(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        /**
         * Parses the internal parser in a loop,
         * in the context of left recursion continuation.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return always true.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            while (true) {
                const bool result = m_parser.parse_left_recursion_continuation(context, context.get_state());
                if (!result) {
                    break;
                }
            }
            return true;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_ZERO_OR_MORE_PARSER_HPP
