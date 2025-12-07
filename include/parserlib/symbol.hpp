#ifndef PARSERLIB_SYMBOL_HPP
#define PARSERLIB_SYMBOL_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    class symbol : public parse_node<symbol<T>> {
    public:
        symbol(const T& value) : m_value(value) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_symbol(m_value);
        }

    private:
        const T m_value;
    };


    template <class T>
    symbol<T> terminal(const T& value) {
        return value;
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_HPP
