#ifndef PARSERLIB_LOGICAL_NOT_HPP
#define PARSERLIB_LOGICAL_NOT_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Child>
    class logical_not : public parse_node<logical_not<Child>> {
    public:
        logical_not(const Child& child) : m_child(child) {
        }

        const Child& get_child() const {
            return m_child;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_logical_not(m_child);
        }

    private:
        const Child m_child;
    };


    template <class Derived>
    logical_not<Derived> parse_node<Derived>::operator !() const {
        return derived();
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_NOT_HPP
