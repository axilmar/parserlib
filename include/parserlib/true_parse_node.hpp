#ifndef PARSERLIB_TRUE_PARSE_NODE_HPP
#define PARSERLIB_TRUE_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class true_parse_node : public parse_node<ParseContext> {
    public:
        bool parse(ParseContext& pc) const override {
            return true;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_TRUE_PARSE_NODE_HPP
