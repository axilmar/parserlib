#ifndef PARSERLIB_REF_PARSE_NODE_HPP
#define PARSERLIB_REF_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class ref_parse_node : public parse_node<ParseContext> {
    public:
        ref_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node.get_shared())
        {
        }

        bool parse(ParseContext& pc) const override {
            std::shared_ptr<parse_node<ParseContext>> shared = m_parse_node.lock();
            return shared->parse(pc);
        }

    private:
        std::weak_ptr<parse_node<ParseContext>> m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_REF_PARSE_NODE_HPP
