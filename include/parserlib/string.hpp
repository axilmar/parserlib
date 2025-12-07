#ifndef PARSERLIB_STRING_HPP
#define PARSERLIB_STRING_HPP


#include <string>
#include <string_view>
#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class string : public parse_node<string<Container>> {
    public:
        string(const Container& container) : m_container(container) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_string(m_container);
        }

    private:
        const Container m_container;
    };


    template <class T>
    string<std::basic_string<T>> terminal(const T* null_terminated_string) {
        return std::basic_string<T>(null_terminated_string);
    }


    template <class T, class Traits, class Alloc>
    string<std::basic_string<T, Traits, Alloc>> terminal(const std::basic_string<T, Traits, Alloc>& str) {
        return str;
    }


    template <class T, class Traits>
    string<std::basic_string_view<T, Traits>> terminal(const std::basic_string_view<T, Traits>& str_view) {
        return str_view;
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_HPP
