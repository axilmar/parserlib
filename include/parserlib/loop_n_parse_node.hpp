#ifndef PARSERLIB_LOOP_N_PARSE_NODE_HPP
#define PARSERLIB_LOOP_N_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext>
    class loop_n_parse_node : public parse_node<ParseContext> {
    public:
        loop_n_parse_node(const parse_node_ptr<ParseContext>& parse_node, size_t times)
            : m_parse_node(parse_node)
            , m_times(times)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto base_state = pc.get_state();
            for (size_t count = 0; count < m_times; ++count) {
                try {
                    if (!m_parse_node->parse(pc)) {
                        pc.set_state(base_state);
                        return false;
                    }
                }
                catch (...) {
                    pc.set_state(base_state);
                    throw;
                }
            }
            return true;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
        size_t m_times;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator * (size_t times, const parse_node_ptr<ParseContext>& parse_node) {
        return std::make_shared<loop_n_parse_node<ParseContext>>(parse_node, times);
    }


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator * (const parse_node_ptr<ParseContext>& parse_node, size_t times) {
        return std::make_shared<loop_n_parse_node<ParseContext>>(parse_node, times);
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_N_PARSE_NODE_HPP
