#ifndef PARSERLIB_UNARY_PARSE_NODE_HPP
#define PARSERLIB_UNARY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Derived, class T>
    class unary_parse_node : public parse_node<Derived> {
    public:
        unary_parse_node(const T& child)
            : m_child(child)
        {
        }

        const T& child() const {
            return m_child;
        }

        const parse_function_type& parse_function() const {
            return m_parse_function;
        }

    private:
        const T m_child;
        const parse_function_type m_parse_function;
    };


} //namespace parserlib


#endif //PARSERLIB_UNARY_PARSE_NODE_HPP
