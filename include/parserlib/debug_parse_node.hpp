#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class debug_parse_node : public interface::parse_node {
    public:
        debug_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            const bool result = m_child.parse(pc);
            return result;
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node debug(const parse_node& pn) {
        return interface::create_parse_node<debug_parse_node>(pn);
    }


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_NODE_HPP
