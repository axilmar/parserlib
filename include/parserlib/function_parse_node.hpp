#ifndef PARSERLIB_FUNCTION_PARSE_NODE_HPP
#define PARSERLIB_FUNCTION_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    class function_parse_node : public interface::parse_node {
    public:
        function_parse_node(const parse_function_type& function) : m_function(function) {
        }

        bool parse(interface::parse_context& pc) const override {
            const bool result = m_function(pc);
            return result;
        }

    private:
        const parse_function_type m_function;
    };


    template <class T>
    parse_node function(T&& fn) {
        return interface::create_parse_node<function_parse_node>(parse_function_type(std::forward<T>(fn)));
    }


    inline parse_node::parse_node(const parse_function_type& fn) : m_parse_node(interface::create_parse_node<function_parse_node>(fn)) {
    }


    inline parse_node::parse_node(parse_function_type&& fn) : m_parse_node(interface::create_parse_node<function_parse_node>(std::move(fn))) {
    }


} //namespace parserlib


#endif //PARSERLIB_FUNCTION_PARSE_NODE_HPP
