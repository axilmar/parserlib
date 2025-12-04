#ifndef PARSERLIB_BOOL_PARSE_NODE_HPP
#define PARSERLIB_BOOL_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class bool_parse_node : public parse_node<bool_parse_node> {
    public:
        bool_parse_node(bool value)
            : parse_node<bool_parse_node>(value ? "true" : "false")
            , m_value(value)
        {
        }

        bool parse(parse_context_interface& pc) const {
            return m_value;
        }

    private:
        const bool m_value;
    };


} //namespace parserlib


#endif //PARSERLIB_BOOL_PARSE_NODE_HPP
