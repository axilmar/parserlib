#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    class symbol_parse_node : public parse_node<symbol_parse_node<T>> {
    public:
        symbol_parse_node(const T& symbol)
            : m_symbol(symbol)
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse_symbol(static_cast<int>(m_symbol));
        }

        const T& symbol() const {
            return m_symbol;
        }

    private:
        const T m_symbol;
    };


    template <class T>
    auto terminal(const T& symbol) {
        return symbol_parse_node<T>(symbol);
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
