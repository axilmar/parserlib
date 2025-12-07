#ifndef PARSERLIB_NEWLINE_HPP
#define PARSERLIB_NEWLINE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Child>
    class newline : public parse_node<newline<Child>> {
    public:
        newline(const Child& child) : m_child(child) {
        }

        const Child& get_child() const {
            return m_child;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_newline(m_child);
        }

    private:
        const Child m_child;
    };


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_HPP
