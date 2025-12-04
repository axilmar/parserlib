#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class string_parse_node : public parse_node<string_parse_node<Container>> {
    public:
        template <class Container>
        string_parse_node(const Container& string)
            : m_string(string)
            , m_symbol_sequence(string.begin(), string.end())
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse_string(m_symbol_sequence);
        }

    private:
        const Container m_string;
        const std::vector<int> m_symbol_sequence;

    };


    template <class T>
    auto terminal(const T* string) {
        return string_parse_node<std::basic_string<T>>(std::basic_string<T>(string));
    }


    template <class T, class Traits, class Alloc>
    auto terminal(const std::basic_string<T, Traits, Alloc>& string) {
        return string_parse_node(string);
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
