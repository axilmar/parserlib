#ifndef PARSERLIB_MATCH_PARSER_HPP
#define PARSERLIB_MATCH_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Parser, class OutputToken>
    class match_parser : public parser<match_parser<Parser, OutputToken>> {
    public:
        match_parser(const Parser& parser, const OutputToken& token)
            : m_parser(parser)
            , m_token(token)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto match_start_state = context.get_state();
            if (m_parser.parse(context)) {
                context.add_match(m_token, match_start_state, context.get_state());
                return true;
            }
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto match_start_state = context.get_state();
            if (m_parser.parse_left_recursion_start(context)) {
                context.add_match(m_token, match_start_state, context.get_state());
                return true;
            }
            return false;
        }

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

