#ifndef PARSERLIB_SET_PARSE_NODE_HPP
#define PARSERLIB_SET_PARSE_NODE_HPP


#include <string>
#include <vector>
#include <array>
#include <set>
#include <unordered_set>
#include <initializer_list>
#include <iterator>
#include "parse_node.hpp"


namespace parserlib {


    template <class Container>
    class set_parse_node : public interface::parse_node {
    public:
        set_parse_node(const Container& container) : m_container(container) {
        }

        set_parse_node(Container&& container) : m_container(std::move(container)) {
        }

        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                const int current_symbol = pc.get_current_symbol();
                for (const auto& set_symbol : m_container) {
                    if (pc.compare_symbols(current_symbol, static_cast<int>(set_symbol)) == 0) {
                        pc.increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

    private:
        const Container m_container;
    };


    template <class T>
    parse_node set(const T* null_term_str) {
        return interface::create_parse_node<set_parse_node<std::basic_string_view<T>>>(null_term_str);
    }


    template <class T, class Traits, class Alloc>
    parse_node set(const std::basic_string<T, Traits, Alloc>& str) {
        return interface::create_parse_node<set_parse_node<std::basic_string<T, Traits, Alloc>>>(str);
    }


    template <class T, class Traits, class Alloc>
    parse_node set(std::basic_string<T, Traits, Alloc>&& str) {
        return interface::create_parse_node<set_parse_node<std::basic_string<T, Traits, Alloc>>>(std::move(str));
    }


    template <class T, class Traits>
    parse_node set(const std::basic_string_view<T, Traits>& str) {
        return interface::create_parse_node<set_parse_node<std::basic_string_view<T, Traits>>>(str);
    }


    template <class T, class Alloc>
    parse_node set(const std::vector<T, Alloc>& vec) {
        return interface::create_parse_node<set_parse_node<std::vector<T, Alloc>>>(vec);
    }


    template <class T, class Alloc>
    parse_node set(std::vector<T, Alloc>&& vec) {
        return interface::create_parse_node<set_parse_node<std::vector<T, Alloc>>>(std::move(vec));
    }


    template <class T, size_t Size>
    parse_node set(const std::array<T, Size>& arr) {
        return interface::create_parse_node<set_parse_node<std::array<T, Size>>>(arr);
    }


    template <class T, size_t Size>
    parse_node set(std::array<T, Size>&& arr) {
        return interface::create_parse_node<set_parse_node<std::array<T, Size>>>(std::move(arr));
    }


    template <class T, class Pred, class Alloc>
    parse_node set(const std::set<T, Pred, Alloc>& set) {
        return interface::create_parse_node<set_parse_node<std::vector<T>>>(std::vector<T>(set.begin(), set.end()));
    }


    template <class T, class Pred, class Alloc>
    parse_node set(std::set<T, Pred, Alloc>&& set) {
        return interface::create_parse_node<set_parse_node<std::vector<T>>>(std::vector<T>(std::make_move_iterator(set.begin()), std::make_move_iterator(set.end())));
    }


    template <class T, class Hash, class KeyComp, class Alloc>
    parse_node set(const std::unordered_set<T, Hash, KeyComp, Alloc>& uset) {
        return interface::create_parse_node<set_parse_node<std::vector<T>>>(std::vector<T>(uset.begin(), uset.end()));
    }


    template <class T, class Hash, class KeyComp, class Alloc>
    parse_node set(std::unordered_set<T, Hash, KeyComp, Alloc>&& uset) {
        return interface::create_parse_node<set_parse_node<std::vector<T>>>(std::vector<T>(std::make_move_iterator(uset.begin()), std::make_move_iterator(uset.end())));
    }


    template <class T>
    parse_node set(const std::initializer_list<T>& ilist) {
        return interface::create_parse_node<set_parse_node<std::vector<T>>>(std::vector<T>(ilist.begin(), ilist.end()));
    }


} //namespace parserlib


#endif //PARSERLIB_SET_PARSE_NODE_HPP
