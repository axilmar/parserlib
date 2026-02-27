#ifndef PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class logical_and_parse_node : public parse_node<ParseContext> {
    public:
        logical_and_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto base_state = pc.get_state();
            try {
                const bool result = m_parse_node->parse(pc);
                pc.set_state(base_state);
                return result;
            }
            catch (...) {
                pc.set_state(base_state);
                throw;
            }
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator &(const parse_node_ptr<ParseContext>& parse_node) {
        if (dynamic_cast<const logical_and_parse_node<ParseContext>*>(parse_node.get())) {
            return parse_node;
        }
        return std::make_shared<logical_and_parse_node<ParseContext>>(parse_node);
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
