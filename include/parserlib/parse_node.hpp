#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include "parse_node_tag.hpp"


namespace parserlib {


    template <class Child> class loop0;
    template <class Child> class loop1;
    template <class Child> class optional;
    template <class Child> class logical_and;
    template <class Child> class logical_not;


    template <class Derived> 
    class parse_node : public parse_node_tag {
    public:
        loop0<Derived> operator *() const;
        loop1<Derived> operator +() const;
        optional<Derived> operator -() const;
        logical_and<Derived> operator &() const;
        logical_not<Derived> operator !() const;
        
        const Derived& derived() const {
            return *static_cast<const Derived*>(this);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
