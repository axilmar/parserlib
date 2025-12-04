#ifndef PARSERLIB_SET_PARSE_NODE_HPP
#define PARSERLIB_SET_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class set_parse_node : public parse_node<set_parse_node<Container>> {
    public:
        set_parse_node(const Container& set)
            : m_set(set)
            , m_symbol_sequence(set.begin(), set.end())
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse_set(m_symbol_sequence);
        }

    private:
        const Container m_set;
        const std::vector<int> m_symbol_sequence;
    };


    template <class T>
    auto set(const T* set) {
        return set_parse_node<std::basic_string<T>>(std::basic_string<T>(set));
    }


    template <class Container>
    auto set(const Container& set) {
        return set_parse_node<Container>(set);
    }


} //namespace parserlib


#endif //PARSERLIB_SET_PARSE_NODE_HPP
