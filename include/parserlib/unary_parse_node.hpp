#ifndef PARSERLIB_UNARY_PARSE_NODE_HPP
#define PARSERLIB_UNARY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Derived, class T>
    class unary_parse_node : public parse_node<Derived> {
    public:
        const T& child() const {
            return m_child;
        }

        const parse_function_type& parse_function() const {
            return m_parse_function;
        }

    protected:
        unary_parse_node(const std::string& type, const T& child)
            : parse_node<Derived>(type)
            , m_child(child)
            , m_parse_function([this](parse_context_interface& pc) { 
                return m_child.parse(pc); 
            })
        {
        }

        ~unary_parse_node() {
        }

    private:
        const T m_child;
        const parse_function_type m_parse_function;
    };


} //namespace parserlib


#endif //PARSERLIB_UNARY_PARSE_NODE_HPP
