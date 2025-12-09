#ifndef PARSERLIB_LOOP0_PARSE_NODE_HPP
#define PARSERLIB_LOOP0_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class loop0_parse_node : public interface::parse_node {
    public:
        loop0_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            while (m_child.parse(pc)) {
            }
            return true;
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node parse_node::operator *() const {
        return interface::create_parse_node<loop0_parse_node>(*this);
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP0_PARSE_NODE_HPP
