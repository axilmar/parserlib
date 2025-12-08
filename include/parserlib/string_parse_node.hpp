#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include <string_view>
#include <string>
#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class string_parse_node : public interface::parse_node {
    public:
        string_parse_node(const Container& container) : m_container(container) {
        }

        string_parse_node(Container&& container) : m_container(std::move(container)) {
        }

        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                pc.push_state();
                auto it = m_container.begin();
                for (;;) {
                    if (it == m_container.end()) {
                        return true;
                    }
                    if (pc.is_end_parse_position() || pc.compare_symbols(pc.get_current_symbol(), static_cast<int>(*it))) {
                        pc.pop_state();
                        return false;
                    }
                    ++it;
                    pc.increment_parse_position();
                }
            }
            return false;
        }

    private:
        const Container m_container;
    };


    template <class T>
    parse_node terminal(const T* null_term_str) {
        return interface::create_parse_node<string_parse_node<std::basic_string_view<T>>>(null_term_str);
    }


    template <class T, class Traits, class Alloc>
    parse_node terminal(const std::basic_string<T, Traits, Alloc>& str) {
        return interface::create_parse_node<string_parse_node<std::basic_string<T, Traits, Alloc>>>(str);
    }


    template <class T, class Traits, class Alloc>
    parse_node terminal(std::basic_string<T, Traits, Alloc>&& str) {
        return interface::create_parse_node<string_parse_node<std::basic_string<T, Traits, Alloc>>>(std::move(str));
    }


    template <class T, class Traits>
    parse_node terminal(const std::basic_string_view<T, Traits>& str) {
        return interface::create_parse_node<string_parse_node<std::basic_string_view<T, Traits>>>(str);
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
