#ifndef PARSERLIB_RULE_REFERENCE_PARSER_NODE_HPP
#define PARSERLIB_RULE_REFERENCE_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class ParseContext> class rule_reference_parser_node : public parser_node<rule_reference_parser_node<ParseContext>> {
    public:
        rule_reference_parser_node(const rule<ParseContext>& r) : m_rule(r) {
        }

        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        const rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif // PARSERLIB_RULE_REFERENCE_PARSER_NODE_HPP
