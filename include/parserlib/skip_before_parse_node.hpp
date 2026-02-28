#ifndef PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
#define PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class skip_before_parse_node : public parse_node<ParseContext> {
    public:
        skip_before_parse_node(const parse_node_ptr<ParseContext>& valid_parse_node, const parse_node_ptr<ParseContext>& invalid_parse_node)
            : m_valid_parse_node(valid_parse_node)
            , m_invalid_parse_node(invalid_parse_node)
        {
        }

        bool parse(ParseContext& pc) const override {
            const auto initial_state = pc.get_state();

            try {
                if (m_invalid_parse_node) {
                    for (;;) {
                        const auto base_state = pc.get_state();

                        const bool invalid_result = m_invalid_parse_node->parse(pc);
                        
                        pc.set_state(initial_state);

                        if (invalid_result) {
                            return false;
                        }

                        const bool valid_result = m_valid_parse_node->parse(pc);
                        
                        pc.set_state(base_state);

                        if (valid_result) {
                            return true;
                        }

                        if (!pc.is_valid_iterator()) {
                            break;
                        }

                        pc.increment_iterator();
                    }
                }

                else {
                    for (;;) {
                        const auto base_state = pc.get_state();

                        const bool result = m_valid_parse_node->parse(pc);
                        
                        pc.set_state(base_state);

                        if (result) {
                            return true;
                        }

                        if (!pc.is_valid_iterator()) {
                            break;
                        }

                        pc.increment_iterator();
                    }
                }
            }

            catch (...) {
                pc.set_state(initial_state);
                throw;
            }

            return false;
        }

    private:
        parse_node_ptr<ParseContext> m_valid_parse_node;
        parse_node_ptr<ParseContext> m_invalid_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
