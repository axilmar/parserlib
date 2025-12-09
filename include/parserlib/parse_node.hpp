#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <utility>
#include <string>
#include <string_view>
#include <type_traits>
#include <functional>
#include "interface_parse_node.hpp"


namespace parserlib {


    using parse_function_type = std::function<bool(interface::parse_context&)>;


    class parse_node {
    public:
        // construction

        parse_node() {
        }

        parse_node(const parse_node& pn) : m_parse_node(pn.m_parse_node) {
        }

        parse_node(parse_node&& pn) noexcept : m_parse_node(std::move(pn.m_parse_node)) {
        }

        parse_node(const interface::parse_node_ptr& ipn) : m_parse_node(ipn) {
        }

        parse_node(interface::parse_node_ptr&& ipn) : m_parse_node(std::move(ipn)) {
        }

        inline parse_node(const parse_function_type& fn);

        inline parse_node(parse_function_type&& fn);

        template <class T>
        parse_node(const T& symbol);

        template <class T>
        parse_node(const T* null_term_str);

        template <class T, class Traits, class Alloc>
        parse_node(const std::basic_string<T, Traits, Alloc>& str);

        template <class T, class Traits, class Alloc>
        parse_node(std::basic_string<T, Traits, Alloc>&& str);

        template <class T, class Traits>
        parse_node(const std::basic_string_view<T, Traits>& str);

        inline parse_node(bool value);

        inline parse_node(rule& r);

        // assignment

        parse_node& operator = (const parse_node& pn) {
            m_parse_node = pn.m_parse_node;
            return *this;
        }

        parse_node& operator = (parse_node&& pn) noexcept {
            m_parse_node = std::move(pn.m_parse_node);
            return *this;
        }

        // DSL

        inline parse_node operator *() const;

        inline parse_node operator +() const;

        inline parse_node operator -() const;

        inline parse_node operator &() const;

        inline parse_node operator !() const;

        // getters

        const interface::parse_node_ptr& get() const {
            return m_parse_node;
        }

        operator const interface::parse_node_ptr& () const {
            return m_parse_node;
        }

        // parse

        bool parse(interface::parse_context& pc) const {
            return m_parse_node->parse(pc);
        }

    private:
        interface::parse_node_ptr m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
