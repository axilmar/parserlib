#ifndef PARSERLIB_LOGICAL_NOT_PARSER_NODE_HPP
#define PARSERLIB_LOGICAL_NOT_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {


    template <class Parser> class logical_not_parser_node : public parser_node<logical_not_parser_node<Parser>> {
    public:
        logical_not_parser_node(const Parser& parser) : m_parser(parser) {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            return pc.parse_and_restore_state([&](ParseContext& pc){ return !m_parser.parse(pc); });
        }

    private:
        Parser m_parser;
    };


    template <class Parser> logical_not_parser_node<Parser> parser_node<Parser>::operator !() const {
        return logical_not_parser_node<Parser>(parser(static_cast<const Parser&>(*this)));
    }


} //namespace parserlib


#endif // PARSERLIB_LOGICAL_NOT_PARSER_NODE_HPP
