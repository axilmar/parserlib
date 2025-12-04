#ifndef PARSERLIB_RULE_PARSE_NODE_HPP
#define PARSERLIB_RULE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class rule;


    class rule_parse_node : public parse_node<rule_parse_node> {
    public:
        rule_parse_node(rule& r);

        bool parse(parse_context_interface& pc) const;

    private:
        rule& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_PARSE_NODE_HPP
