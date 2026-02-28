#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    class any_parse_node : public parse_node<ParseContext> {
    public:
        bool parse(ParseContext& pc) const override {
            if (pc.is_valid_iterator()) {
                pc.increment_iterator();
                return true;
            }
            return false;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ANY_PARSE_NODE_HPP
