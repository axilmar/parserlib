#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class any_parse_node : public interface::parse_node {
    public:
        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        }
    };


    inline parse_node any() {
        return interface::create_parse_node<any_parse_node>();
    }


} //namespace parserlib


#endif //PARSERLIB_ANY_PARSE_NODE_HPP
