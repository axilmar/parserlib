#ifndef PARSERLIB_LOOP_0_PARSE_NODE_HPP
#define PARSERLIB_LOOP_0_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T>
    class loop_0_parse_node : public unary_parse_node<loop_0_parse_node<T>, T> {
    public:
        using unary_parse_node<loop_0_parse_node<T>, T>::unary_parse_node;

        bool parse(parse_context_interface& pc) const {
            return pc.parse_loop(this->parse_function());
        }
    };


    template <class Derived>
    loop_0_parse_node<Derived> parse_node<Derived>::operator *() const {
        return loop_0_parse_node<Derived>(*static_cast<const Derived *>(this));
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_0_PARSE_NODE_HPP
