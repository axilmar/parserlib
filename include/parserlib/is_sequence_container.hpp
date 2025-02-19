#ifndef PARSERLIB_IS_SEQUENCE_CONTAINER_HPP
#define PARSERLIB_IS_SEQUENCE_CONTAINER_HPP


#include <vector>
#include <string>
#include <string_view>
#include <list>
#include <array>
#include <initializer_list>
#include <type_traits>


namespace parserlib {


    template <class T> struct is_sequence_container {
        static constexpr bool value = false;
    };


    template <class Elem, class Alloc> struct is_sequence_container<std::vector<Elem, Alloc>> {
        static constexpr bool value = true;
    };


    template <class Elem, class Traits, class Alloc> struct is_sequence_container<std::basic_string<Elem, Traits, Alloc>> {
        static constexpr bool value = true;
    };


    template <class Elem, class Traits> struct is_sequence_container<std::basic_string_view<Elem, Traits>> {
        static constexpr bool value = true;
    };


    template <class Elem, class Alloc> struct is_sequence_container<std::list<Elem, Alloc>> {
        static constexpr bool value = true;
    };


    template <class Elem, std::size_t Size> struct is_sequence_container<std::array<Elem, Size>> {
        static constexpr bool value = true;
    };


    template <class T> struct is_sequence_container<std::initializer_list<T>> {
        static constexpr bool value = true;
    };


    template <class T>
    constexpr bool is_sequence_container_v = is_sequence_container<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_SEQUENCE_CONTAINER_HPP
