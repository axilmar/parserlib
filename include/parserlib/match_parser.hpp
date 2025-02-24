#ifndef PARSERLIB_MATCH_PARSER_HPP
#define PARSERLIB_MATCH_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that adds a match to the parse context,
     * if the internal parser suceeds in parsing.
     * @param Parser the internal parser.
     * @param OutputToken of the id of the match.
     */
    template <class Parser, class OutputToken>
    class match_parser : public parser<match_parser<Parser, OutputToken>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser.
         * @param token the output token, a.k.a. the id of the match.
         */
        match_parser(const Parser& parser, const OutputToken& token)
            : m_parser(parser)
            , m_token(token)
        {
        }

        /**
         * Invokes the internal parser, if that succeeds,
         * it adds a match to the parse context.
         * @param context the parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto match_start_state = context.get_state();
            if (m_parser.parse(context)) {
                context.add_match(m_token, match_start_state, context.get_state());
                return true;
            }
            return false;
        }

        /**
         * Invokes the internal parser, if that succeeds,
         * it adds a match to the parse context,
         * in the context of left recursion start.
         * @param context the parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto match_start_state = context.get_state();
            if (m_parser.parse_left_recursion_start(context)) {
                context.add_match(m_token, match_start_state, context.get_state());
                return true;
            }
            return false;
        }

        /**
         * Invokes the internal parser, if that succeeds,
         * it adds a match to the parse context,
         * in the context of left recursion continuation.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if (m_parser.parse_left_recursion_continuation(context, context.get_state())) {
                context.add_match(m_token, match_start_state, context.get_state());
                return true;
            }
            return false;
        }

    private:
        const Parser m_parser;
        const OutputToken m_token;
    };


    template <class Parser, class OutputToken>
    match_parser<Parser, OutputToken> operator ->* (const Parser& parser, const OutputToken& token) {
        return { parser, token };
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_PARSER_HPP

