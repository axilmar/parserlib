#ifndef PARSERLIB_OPTIONAL_PARSE_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T>
    class optional_parse_node : public unary_parse_node<optional_parse_node<T>, T> {
    public:
        optional_parse_node(const T& child) 
            : unary_parse_node<optional_parse_node<T>, T>('-' + child.type(), child)
        {
        }

        bool parse(parse_context_interface& pc) const {
            this->child().parse(pc);
            return true;
        }
    };


    template <class Derived>
    optional_parse_node<Derived> parse_node<Derived>::operator -() const {
        return optional_parse_node<Derived>(*static_cast<const Derived *>(this));
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
