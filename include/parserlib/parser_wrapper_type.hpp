#ifndef PARSERLIB_PARSER_WRAPPER_TYPE_HPP
#define PARSERLIB_PARSER_WRAPPER_TYPE_HPP


#include <type_traits>
#include "is_container.hpp"


namespace parserlib {


    template <class Derived> class parser;
    template <class ParseContext> class rule;
    template <class ParseContext> class rule_reference;
    template <class Terminal> class terminal_parser;
    template <class Terminal> class terminal_string_parser;


    /**
     * Returns a parser wrapper for rules.
     * In order to allow recursion, rules are managed via references, wrapped in a `rule_reference` class.
     * @param rule rule to get the parser wrapper for.
     * @return a rule_reference object for this rule.
     */
    template <class ParseContext>
    rule_reference<ParseContext> get_parser_wrapper(const parser<rule<ParseContext>>& rule) {
        return rule_reference(*rule.pointer_to_derived());
    }


    /**
     * Returns a parser wrapper for parsers other than rules.
     * Parser objects other than rules do not have a special parser wrapper class.
     * @param parser the parser object to get a wrapper for.
     * @return the parser object given as input.
     */
    template <class Derived>
    const Derived& get_parser_wrapper(const parser<Derived>& parser) {
        return static_cast<const Derived&>(parser);
    }


    /**
     * Returns a parser for a container of tokens.
     * The container is treated as a string (or it might be a string class).
     * @param container the container.
     * @return a terminal string parser for the given container.
     */
    template <class T, std::enable_if_t<is_container_v<T>, bool> = true>
    auto get_parser_wrapper(const T& container) {
        return terminal_string_parser<typename T::value_type>(container.begin(), container.end());
    }


    /**
     * Returns a parser for a null-terminated string.
     * @param str null-terminated string to get a parser for.
     * @return a terminal string parser for the given null-terminated string.
     */
    template <class T>
    auto get_parser_wrapper(const T* str) {
        return terminal_string_parser<T>(str);
    }


    /**
     * Returns a parser for an array of tokens.
     * @param arr array of tokens.
     * @return a terminal string parser for the given array.
     */
    template <class T, std::size_t Size>
    auto get_parser_wrapper(const T arr[Size]) {
        return terminal_string_parser<typename T::value_type>(arr, arr + Size);
    }


    /**
     * Returns a parser wrapper for an object that it is not a rule, not a parser, and not a container.
     * @param obj object.
     * @return a terminal value parser for this object.
     */
    template <class T, std::enable_if_t<!std::is_base_of_v<parser<T>, T> && !is_container_v<T>, bool> = true>
    terminal_parser<T> get_parser_wrapper(const T& obj) {
        return obj;
    }


    /**
     * Returns the type for a parser wrapper.
     * @param T the type of object to get the parser wrapper type of.
     */
    template <class T>
    using parser_wrapper_type = std::remove_const_t<std::remove_reference_t<decltype(get_parser_wrapper(*(const T*)(nullptr)))>>;


} //namespace parserlib


#endif //PARSERLIB_PARSER_WRAPPER_TYPE_HPP
