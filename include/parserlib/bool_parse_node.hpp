#ifndef PARSERLIB_BOOL_PARSE_NODE_HPP
#define PARSERLIB_BOOL_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class bool_parse_node : public interface::parse_node {
    public:
        bool_parse_node(bool value) : m_value(value) {
        }

        bool parse(interface::parse_context& pc) const override {
            return m_value;
        }

    private:
        const bool m_value;
    };


    inline parse_node::parse_node(bool value) : m_parse_node(interface::create_parse_node<bool_parse_node>(value)) {
    }


} //namespace parserlib


#endif //PARSERLIB_BOOL_PARSE_NODE_HPP
