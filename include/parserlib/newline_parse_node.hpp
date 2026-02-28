#ifndef PARSERLIB_NEWLINE_PARSE_NODE_HPP
#define PARSERLIB_NEWLINE_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class newline_parse_node : public parse_node<ParseContext> {
    public:
        newline_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            if (m_parse_node->parse(pc)) {
                pc.increment_line();
                return true;
            }
            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSE_NODE_HPP
