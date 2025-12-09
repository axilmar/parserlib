#ifndef PARSERLIB_OPTIONAL_PARSE_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class optional_parse_node : public interface::parse_node {
    public:
        optional_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            m_child.parse(pc);
            return true;
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node parse_node::operator -() const {
        return interface::create_parse_node<optional_parse_node>(*this);
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
