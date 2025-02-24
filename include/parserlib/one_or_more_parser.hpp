#ifndef PARSERLIB_ONE_OR_MORE_PARSER_HPP
#define PARSERLIB_ONE_OR_MORE_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that repeats the internal parser one or more times.
     * The internal parser must parse successfully at least once.
     * @param Parser the internal parser.
     */
    template <class Parser>
    class one_or_more_parser : public parser<one_or_more_parser<Parser>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         */
        one_or_more_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Invokes the internal parser in a loop.
         * @param context parse context.
         * @return true if the internal parser parses successfully at least once, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (!m_parser.parse(context)) {
                return false;
            }
            while (true) {
                const bool result = m_parser.parse(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        /**
         * Invokes the internal parser in a loop,
         * in the context of left recursion start.
         * @param context parse context.
         * @return true if the internal parser parses successfully at least once, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            if (!m_parser.parse_left_recursion_start(context)) {
                return false;
            }
            while (true) {
                const bool result = m_parser.parse_left_recursion_start(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        /**
         * Invokes the internal parser in a loop,
         * in the context of left recursion continuation.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if the internal parser parses successfully at least once, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if (!m_parser.parse_left_recursion_continuation(context, context.get_state())) {
                return false;
            }
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


#endif //PARSERLIB_ONE_OR_MORE_PARSER_HPP
