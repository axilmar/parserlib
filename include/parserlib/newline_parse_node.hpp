#ifndef PARSERLIB_NEWLINE_PARSE_NODE_HPP
#define PARSERLIB_NEWLINE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class newline_parse_node : public interface::parse_node {
    public:
        newline_parse_node(const parserlib::parse_node& child) : m_child(child) {
        }

        bool parse(interface::parse_context& pc) const override {
            if (m_child.parse(pc)) {
                pc.increment_parse_position_line();
                return true;
            }
            return false;
        }

    private:
        const parserlib::parse_node m_child;
    };


    inline parse_node newline(const parse_node& pn) {
        return interface::create_parse_node<newline_parse_node>(pn);
    }


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSE_NODE_HPP
