#ifndef PARSERLIB_RULE_PARSE_NODE_HPP
#define PARSERLIB_RULE_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class rule_parse_node : public parse_node<ParseContext> {
    public:
        rule_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node.get_shared())
        {
        }

        bool parse(ParseContext& pc) const override {
            return pc.parse_left_recursion(m_parse_node.get());
        }

    private:
        std::shared_ptr<parse_node<ParseContext>> m_parse_node;

        friend class rule<ParseContext>;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_PARSE_NODE_HPP
