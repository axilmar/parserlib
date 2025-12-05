#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <string>
#include <utility>
#include "parse_context_interface.hpp"


namespace parserlib {


    template <class T> class loop_0_parse_node;
    template <class T> class loop_1_parse_node;
    template <class T> class optional_parse_node;
    template <class T> class logical_and_parse_node;
    template <class T> class logical_not_parse_node;


    struct parse_node_tag {
    };


    template <class Derived>
    class parse_node : public parse_node_tag {
    public:
        loop_0_parse_node<Derived> operator *() const;

        loop_1_parse_node<Derived> operator +() const;

        optional_parse_node<Derived> operator -() const;

        logical_and_parse_node<Derived> operator &() const;

        logical_not_parse_node<Derived> operator !() const;

    protected:
        parse_node() {
        }

        ~parse_node() {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
