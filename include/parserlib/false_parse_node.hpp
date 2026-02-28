#ifndef PARSERLIB_FALSE_PARSE_NODE_HPP
#define PARSERLIB_FALSE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class false_parse_node : public parse_node<ParseContext> {
    public:
        bool parse(ParseContext& pc) const override {
            return false;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_FALSE_PARSE_NODE_HPP
