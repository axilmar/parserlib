#ifndef PARSERLIB_END_PARSE_NODE_HPP
#define PARSERLIB_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class end_parse_node : public parse_node<ParseContext> {
    public:
        bool parse(ParseContext& pc) const override {
            return !pc.is_valid_iterator();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_END_PARSE_NODE_HPP
