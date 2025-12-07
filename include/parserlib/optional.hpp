#ifndef PARSERLIB_OPTIONAL_HPP
#define PARSERLIB_OPTIONAL_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Child>
    class optional : public parse_node<optional<Child>> {
    public:
        optional(const Child& child) : m_child(child) {
        }

        const Child& get_child() const {
            return m_child;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_optional(m_child);
        }

    private:
        const Child m_child;
    };


    template <class Derived>
    optional<Derived> parse_node<Derived>::operator -() const {
        return derived();
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_HPP
