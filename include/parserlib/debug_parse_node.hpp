#ifndef PARSERLIB_DEBUG_PARSE_NODE_HPP
#define PARSERLIB_DEBUG_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T>
    class debug_parse_node : public unary_parse_node<debug_parse_node<T>, T> {
    public:
        debug_parse_node(const T& child) 
            : unary_parse_node<debug_parse_node<T>, T>(child)
        {
        }

        bool parse(parse_context_interface& pc) const {
            const bool ok = this->child().parse(pc);
            return ok;
        }
    };


    template <class Derived>
    auto debug(const parse_node<Derived>& parse_node) {
        return debug_parse_node<Derived>(static_cast<const Derived&>(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_NODE_HPP
