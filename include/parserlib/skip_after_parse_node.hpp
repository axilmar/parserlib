#ifndef PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
#define PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class skip_after_parse_node : public interface::parse_node {
    public:
        skip_after_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                do {
                    pc.push_state();
                    try {
                        const bool result = m_child.parse(pc);
                        if (result) {
                            pc.pop_state_without_activation();
                            return true;
                        }
                    }
                    catch (...) {
                        pc.pop_state();
                        throw;
                    }
                    pc.pop_state();
                    pc.increment_parse_position();
                } while (pc.is_valid_parse_position());
            }
            return false;
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node skip_after(const parse_node& pn) {
        return interface::create_parse_node<skip_after_parse_node>(pn);
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_AFTER_PARSE_NODE_HPP
