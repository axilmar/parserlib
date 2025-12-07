#ifndef PARSERLIB_END_HPP
#define PARSERLIB_END_HPP


#include "parse_node.hpp"


namespace parserlib {


    class end : public parse_node<end> {
    public:
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_end();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_END_HPP
