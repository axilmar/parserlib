#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class debug_parse_node : public parse_node<ParseContext> {
    public:
        debug_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            const bool result = m_parse_node->parse(pc);
            return result;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_NODE_HPP
