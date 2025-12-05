#ifndef PARSERLIB_MATCH_START_PARSE_NODE_HPP
#define PARSERLIB_MATCH_START_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class match_start_parse_node : public parse_node_base {
    public:
        bool parse(parse_context_interface& pc) const {
            pc.push_match_start_state();
            return true;
        }
    };


    inline match_start_parse_node match_start() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_START_PARSE_NODE_HPP
