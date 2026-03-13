#ifndef PARSERLIB_GET_SOURCE_HPP
#define PARSERLIB_GET_SOURCE_HPP


#include <type_traits>
#include <algorithm>
#include <string>
#include "match.hpp"
#include "error.hpp"


namespace parserlib {


    template <class Char> struct get_char_source {
        template <class Iterator>
        static std::basic_string<Char> get_source(const Iterator& begin, const Iterator& end) {
            return { begin, end };
        }
    };


    template <class T>
    struct get_source_impl;


    template <> struct get_source_impl<signed char> : get_char_source<signed char> {};
    template <> struct get_source_impl<unsigned char> : get_char_source<unsigned char> {};
    template <> struct get_source_impl<char> : get_char_source<char> {};
    template <> struct get_source_impl<wchar_t> : get_char_source<wchar_t> {};
    #ifdef char8_t
    template <> struct get_source_impl<char8_t> : get_char_source<char8_t> {};
    #endif
    template <> struct get_source_impl<char16_t> : get_char_source<char16_t> {};
    template <> struct get_source_impl<char32_t> : get_char_source<char32_t> {};


    template <class Iterator, class Id>
    struct get_source_impl<source_partition<Iterator, Id>> {
        template <class Iterator1>
        static auto get_source(const Iterator1& begin, const Iterator1& end) {
            using value_type = std::decay_t<typename Iterator::value_type>;
            return get_source_impl<value_type>::get_source(begin->begin(), begin != end ? std::prev(end)->end() : end->end());
        }
    };


    template <class Iterator, class Id>
    struct get_source_impl<match<Iterator, Id>> : get_source_impl<source_partition<Iterator, Id>> {
    };


    template <class Iterator, class Id>
    struct get_source_impl<error<Iterator, Id>> : get_source_impl<source_partition<Iterator, Id>> {
    };


    template <class Iterator, class Id>
    auto get_source(const source_partition<Iterator, Id>& partition) {
        using value_type = std::decay_t<typename Iterator::value_type>;
        return get_source_impl<value_type>::get_source(partition.begin(), partition.end());
    }


    template <class Iterator, class Id>
    auto source_partition<Iterator, Id>::get_source() const {
        return parserlib::get_source(*this);
    }


} //namespace parserlib


#endif //PARSERLIB_GET_SOURCE_HPP
