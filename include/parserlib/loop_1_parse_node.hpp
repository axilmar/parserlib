#ifndef PARSERLIB_LOOP_1_PARSE_NODE_HPP
#define PARSERLIB_LOOP_1_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class loop_1_parse_node : public parse_node<ParseContext> {
    public:
        loop_1_parse_node(const parse_node_ptr<ParseContext>& parse_node)
            : m_parse_node(parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            if (m_parse_node->parse(pc)) {
                for (;;) {
                    const auto base_state = pc.get_state();
                    try {
                        if (!m_parse_node->parse(pc) || pc.get_iterator() == base_state.get_iterator()) {
                            pc.set_state(base_state);
                            break;
                        }
                    }
                    catch (...) {
                        pc.set_state(base_state);
                        throw;
                    }
                }
                return true;
            }
            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator +(const parse_node_ptr<ParseContext>& parse_node) {
        if (dynamic_cast<const loop_1_parse_node<ParseContext>*>(parse_node.get())) {
            return parse_node;
        }
        return std::make_shared<loop_1_parse_node<ParseContext>>(parse_node);
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_1_PARSE_NODE_HPP
