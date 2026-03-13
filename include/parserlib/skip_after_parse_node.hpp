#ifndef PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
#define PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class skip_after_parse_node : public parse_node<ParseContext> {
    public:
        skip_after_parse_node(const parse_node_ptr<ParseContext>& valid_parse_node)
            : m_parse_node(valid_parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto initial_state = pc.get_state();

            try {
                for (;;) {
                    const auto base_state = pc.get_state();

                    const bool result = m_parse_node->parse(pc);

                    if (result) {
                        return true;
                    }

                    pc.set_state(base_state);

                    if (!pc.is_valid_iterator()) {
                        break;
                    }

                    pc.increment_iterator();
                }
            }

            catch (...) {
                pc.set_state(initial_state);
                throw;
            }

            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
