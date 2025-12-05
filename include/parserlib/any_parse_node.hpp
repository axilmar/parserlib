#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class any_parse_node : public parse_node<any_parse_node> {
    public:
        bool parse(parse_context_interface& pc) const {
            return pc.parse_any();
        }
    };


    inline any_parse_node any() {
        return any_parse_node();
    }


} //namespace parserlib


#endif //PARSERLIB_ANY_PARSE_NODE_HPP
