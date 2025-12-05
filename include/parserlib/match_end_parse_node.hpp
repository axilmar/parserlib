#ifndef PARSERLIB_MATCH_END_PARSE_NODE_HPP
#define PARSERLIB_MATCH_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class match_end_parse_node : public parse_node_base {
    public:
        bool parse(parse_context_interface& pc) const {
            pc.pop_match_start_state();
            return true;
        }
    };


    inline match_end_parse_node match_end() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_END_PARSE_NODE_HPP
