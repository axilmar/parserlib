#ifndef PARSERLIB_END_PARSE_NODE_HPP
#define PARSERLIB_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class end_parse_node : public parse_node<end_parse_node> {
    public:
        bool parse(parse_context_interface& pc) const {
            return pc.is_parse_end();
        }
    };


    inline end_parse_node end() {
        return end_parse_node();
    }


} //namespace parserlib


#endif //PARSERLIB_END_PARSE_NODE_HPP
