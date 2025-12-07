#ifndef PARSERLIB_SET_HPP
#define PARSERLIB_SET_HPP


#include <string>
#include <initializer_list>
#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class set_ : public parse_node<set_<Container>> {
    public:
        set_(const Container& container) : m_container(container) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_set(m_container);
        }

    private:
        const Container m_container;
    };


    template <class T>
    set_<std::basic_string<T>> set(const T* null_terminated_string) {
        return std::basic_string<T>(null_terminated_string);
    }


    template <class Container>
    set_<Container> set(const Container& container) {
        return container;
    }


    template <class T>
    set_<std::vector<T>> set(const std::initializer_list<T>& init_list) {
        return std::vector<T>(init_list);
    }


} //namespace parserlib


#endif //PARSERLIB_SET_HPP
