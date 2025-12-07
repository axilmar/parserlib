#ifndef PARSERLIB_LOOP0_HPP
#define PARSERLIB_LOOP0_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Child>
    class loop0 : public parse_node<loop0<Child>> {
    public:
        loop0(const Child& child) : m_child(child) {
        }

        const Child& get_child() const {
            return m_child;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_loop0(m_child);
        }

    private:
        const Child m_child;
    };


    template <class Derived>
    loop0<Derived> parse_node<Derived>::operator *() const {
        return derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP0_HPP
