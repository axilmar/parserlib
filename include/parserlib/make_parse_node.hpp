#ifndef PARSERLIB_MAKE_PARSE_NODE_HPP
#define PARSERLIB_MAKE_PARSE_NODE_HPP


#include <type_traits>
#include "bool.hpp"
#include "symbol.hpp"
#include "string.hpp"


namespace parserlib {


    template <class Derived>
    const Derived& make_parse_node(const parse_node<Derived>& parse_node) {
        return parse_node.derived();
    }


    template <class T, std::enable_if_t<!std::is_same_v<T, bool> && !std::is_base_of_v<parse_node_tag, T>, bool> = true>
    symbol<T> make_parse_node(const T& value) {
        return value;
    }


    template <class T>
    string<std::basic_string<T>> make_parse_node(const T* null_terminated_string) {
        return std::basic_string<T>(null_terminated_string);
    }


    template <class T, class Traits, class Alloc>
    string<std::basic_string<T, Traits, Alloc>> make_parse_node(const std::basic_string<T, Traits, Alloc>& str) {
        return str;
    }


    template <class T, class Traits>
    string<std::basic_string_view<T, Traits>> make_parse_node(const std::basic_string_view<T, Traits>& str_view) {
        return str_view;
    }


    template <class T, std::enable_if_t<std::is_same_v<T, bool>> = true>
    inline bool_ make_parse_node(const T& value) {
        return value;
    }


    template <class ParseContext> class rule;


    template <class ParseContext>
    rule_ref<ParseContext> make_parse_node(rule<ParseContext>& r) {
        return r;
    }


} //namespace parserlib


#endif //PARSERLIB_MAKE_PARSE_NODE_HPP
