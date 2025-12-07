#ifndef PARSERLIB_ANY_HPP
#define PARSERLIB_ANY_HPP


#include "parse_node.hpp"


namespace parserlib {


    class any : public parse_node<any> {
    public:
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_any();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ANY_HPP
