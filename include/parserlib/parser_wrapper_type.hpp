#ifndef PARSERLIB_PARSER_WRAPPER_TYPE_HPP
#define PARSERLIB_PARSER_WRAPPER_TYPE_HPP


#include "is_sequence_container.hpp"


namespace parserlib {


    template <class Derived> class parser;
    template <class ParseContext> class rule;
    template <class ParseContext> class rule_reference;
    template <class Terminal> class terminal_parser;
    template <class Terminal> class terminal_string_parser;


    template <class ParseContext>
    rule_reference<ParseContext> get_parser_wrapper(const parser<rule<ParseContext>>& rule) noexcept {
        return rule_reference(*rule.pointer_to_derived());
    }


    template <class Derived>
    const Derived& get_parser_wrapper(const parser<Derived>& parser) noexcept {
        return static_cast<const Derived&>(parser);
    }


    template <class T, std::enable_if_t<is_sequence_container_v<T>, bool> = true>
    auto get_parser_wrapper(const T& container) noexcept {
        return terminal_string_parser<T::value_type>(container.begin(), container.end());
    }


    template <class T>
    auto get_parser_wrapper(const T* str) noexcept {
        return terminal_string_parser<T::value_type>(str);
    }


    template <class T, std::size_t Size>
    auto get_parser_wrapper(const T arr[Size]) noexcept {
        return terminal_string_parser<T::value_type>(arr, arr + Size);
    }


    template <class T, std::enable_if_t<!std::is_base_of_v<parser<T>, T> && !is_sequence_container_v<T>, bool> = true>
    terminal_parser<T> get_parser_wrapper(const T& obj) noexcept {
        return obj;
    }


    template <class T>
    using parser_wrapper_type = std::remove_const_t<std::remove_reference_t<decltype(get_parser_wrapper(*(const T*)(nullptr)))>>;


} //namespace parserlib


#endif //PARSERLIB_PARSER_WRAPPER_TYPE_HPP
