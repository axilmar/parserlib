#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    class symbol_parse_node : public interface::parse_node {
    public:
        symbol_parse_node(const T& symbol) : m_symbol(symbol) {
        }

        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                if (pc.compare_symbols(pc.get_current_symbol(), static_cast<int>(m_symbol)) == 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

    private:
        const T m_symbol;
    };


    template <class T>
    parse_node terminal(const T& symbol) {
        return interface::create_parse_node<symbol_parse_node<T>>(symbol);
    }


    template <class T>
    parse_node::parse_node(const T& symbol) : m_parse_node(terminal(symbol)) {
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
