#ifndef PARSERLIB_MAKE_PARSE_NODE_HPP
#define PARSERLIB_MAKE_PARSE_NODE_HPP


#include <type_traits>
#include <string>
#include "symbol_parse_node.hpp"
#include "string_parse_node.hpp"
#include "bool_parse_node.hpp"
#include "rule_parse_node.hpp"


namespace parserlib {


    //parse node
    template <class T, std::enable_if_t<std::is_base_of_v<parse_node_base, T>, bool> = true>
    const T& make_parse_node(const T& parse_node) {
        return parse_node;
    }


    //non-bool integral
    template <class T, std::enable_if_t<!std::is_same_v<T, bool> && std::is_integral_v<T>, bool> = true>
    auto make_parse_node(const T& value) {
        return symbol_parse_node<T>(value);
    }


    //enum
    template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    auto make_parse_node(const T& value) {
        return symbol_parse_node<T>(value);
    }


    //pointer
    template <class T>
    auto make_parse_node(const T* value) {
        return string_parse_node<std::basic_string<T>>(std::basic_string<T>(value));
    }


    //std::basic_string
    template <class T, class Traits, class Alloc>
    auto make_parse_node(const std::basic_string<T, Traits, Alloc>& value) {
        return string_parse_node<std::basic_string<T, Traits, Alloc>>(value);
    }


    //bool
    template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
    auto make_parse_node(const T& value) {
        return bool_parse_node(value);
    }


    //rule
    inline rule_parse_node make_parse_node(rule& r) {
        return r;
    }


} //namespace parserlib


#endif //PARSERLIB_MAKE_PARSE_NODE_HPP
