#ifndef PARSERLIB_MATCH_PARSER_NODE_HPP
#define PARSERLIB_MATCH_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {



    template <class Parser, class MatchId> class match_parser_node : public parser_node<match_parser_node<Parser, MatchId>> {
    public:
        match_parser_node(const Parser& parser, const MatchId& match_id) : m_parser(parser), m_match_id(match_id) {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            return pc.parse_match(m_parser, m_match_id);
        }

    private:
        Parser m_parser;
        MatchId m_match_id;
    };


    template <class Parser, class MatchId> auto operator ->* (const Parser& p, const MatchId& match_id) {
        return match_parser_node(parser(p), match_id);
    }


} //namespace parserlib


#endif // PARSERLIB_MATCH_PARSER_NODE_HPP
