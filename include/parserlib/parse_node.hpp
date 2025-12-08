#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include "interface_parse_node.hpp"


namespace parserlib {


    class parse_node {
    public:
        // construction

        parse_node() {
        }

        parse_node(parse_node&& pn) noexcept : m_parse_node(std::move(pn.m_parse_node)) {
        }

        parse_node(interface::parse_node_ptr&& ipn) : m_parse_node(std::move(ipn)) {
        }

        // assignment

        parse_node& operator = (parse_node&& pn) noexcept {
            m_parse_node = std::move(pn.m_parse_node);
            return *this;
        }

        parse_node& operator = (interface::parse_node_ptr&& ipn) {
            m_parse_node = std::move(ipn);
            return *this;
        }

        // DSL

        inline parse_node operator *() const;

        inline parse_node operator +() const;

        inline parse_node operator -() const;

        inline parse_node operator &() const;

        inline parse_node operator !() const;

        // parse

        bool parse(interface::parse_context& pc) const {
            return m_parse_node->parse(pc);
        }

    private:
        interface::parse_node_ptr m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
