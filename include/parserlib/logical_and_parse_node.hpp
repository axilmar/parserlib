#ifndef PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class logical_and_parse_node : public interface::parse_node {
    public:
        logical_and_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            pc.push_state();
            try {
                const bool result = m_child.parse(pc);
                pc.restore_state();
                return result;
            }
            catch (...) {
                pc.restore_state();
                throw;
            }
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node parse_node::operator &() const {
        return interface::create_parse_node<logical_and_parse_node>(*this);
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
