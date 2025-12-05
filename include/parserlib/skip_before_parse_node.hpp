#ifndef PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
#define PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T>
    class skip_before_parse_node : public unary_parse_node<skip_before_parse_node<T>, T> {
    public:
        skip_before_parse_node(const T& child) 
            : unary_parse_node<skip_before_parse_node<T>, T>(child)
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.skip_before(this->parse_function());
        }
    };


    template <class T>
    auto skip_before(T&& child) {
        return skip_before_parse_node(make_parse_node(child));
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_BEFORE_PARSE_NODE_HPP
