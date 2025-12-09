#ifndef PARSERLIB_END_PARSE_NODE_HPP
#define PARSERLIB_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class end_parse_node : public interface::parse_node {
    public:
        bool parse(interface::parse_context& pc) const override {
            return pc.is_end_parse_position();
        }
    };


    inline parse_node end() {
        return interface::create_parse_node<end_parse_node>();
    }


} //namespace parserlib


#endif //PARSERLIB_END_PARSE_NODE_HPP
