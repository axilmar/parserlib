#ifndef PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T>
    class logical_and_parse_node : public unary_parse_node<logical_and_parse_node<T>, T> {
    public:
        using unary_parse_node<logical_and_parse_node<T>, T>::unary_parse_node;

        bool parse(parse_context_interface& pc) const {
            return pc.test_parse(this->parse_function());
        }
    };


    template <class Derived>
    logical_and_parse_node<Derived> parse_node<Derived>::operator &() const {
        return logical_and_parse_node<Derived>(*static_cast<const Derived *>(this));
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_AND_PARSE_NODE_HPP
