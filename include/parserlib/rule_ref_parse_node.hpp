#ifndef PARSERLIB_RULE_REF_PARSE_NODE_HPP
#define PARSERLIB_RULE_REF_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class rule_ref_parse_node : public interface::parse_node {
    public:
        rule_ref_parse_node(rule& r) : m_rule(r) {
        }

        bool parse(interface::parse_context& pc) const override;

    private:
        rule& m_rule;
    };


    inline parse_node rule_ref(rule& r) {
        return interface::create_parse_node<rule_ref_parse_node>(r);
    }


    inline parse_node::parse_node(rule& r) : m_parse_node(rule_ref(r)) {
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_REF_PARSE_NODE_HPP
