#ifndef PARSERLIB_LOOP1_HPP
#define PARSERLIB_LOOP1_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Child>
    class loop1 : public parse_node<loop1<Child>> {
    public:
        loop1(const Child& child) : m_child(child) {
        }

        const Child& get_child() const {
            return m_child;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_loop1(m_child);
        }

    private:
        const Child m_child;
    };


    template <class Derived>
    loop1<Derived> parse_node<Derived>::operator +() const {
        return derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP1_HPP
