#ifndef PARSERLIB_SYMBOL_SET_PARSER_NODE_HPP
#define PARSERLIB_SYMBOL_SET_PARSER_NODE_HPP


#include <string>
#include <algorithm>
#include "parser_node.hpp"


namespace parserlib {


    template <class Symbol> class symbol_set_parser_node : public parser_node<symbol_set_parser_node<Symbol>> {
    public:
        using string_type = std::basic_string<Symbol>;

        symbol_set_parser_node(const string_type& string) : m_string(string) {
            std::sort(m_string.begin(), m_string.end());
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            return pc.parse_symbol_set(m_string);
        }

    private:
        string_type m_string;
    };


    template <class Symbol> symbol_set_parser_node<Symbol> set(const Symbol* string) {
        return symbol_set_parser_node<Symbol>(string);
    }


    template <class Symbol> symbol_set_parser_node<Symbol> set(const std::basic_string<Symbol>& string) {
        return symbol_set_parser_node<Symbol>(string);
    }


} //namespace parserlib


#endif // PARSERLIB_SYMBOL_SET_PARSER_NODE_HPP
