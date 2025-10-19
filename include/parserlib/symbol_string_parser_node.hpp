#ifndef PARSERLIB_SYMBOL_STRING_PARSER_NODE_HPP
#define PARSERLIB_SYMBOL_STRING_PARSER_NODE_HPP


#include <string>
#include "parser_node.hpp"


namespace parserlib {


    template <class Symbol> class symbol_string_parser_node : public parser_node<symbol_string_parser_node<Symbol>> {
    public:
        using string_type = std::basic_string<Symbol>;

        symbol_string_parser_node(const string_type& string) : m_string(string) {
        }

        template <class ParseContext> bool parse(const ParseContext& pc) const {
            return pc.parse_symbol_string(m_string);
        }

    private:
        string_type m_string;
    };


    template <class Symbol> symbol_string_parser_node<Symbol> parser(const Symbol* string) {
        return symbol_string_parser_node<Symbol>(string);
    }


    template <class Symbol> symbol_string_parser_node<Symbol> parser(const std::basic_string<Symbol>& string) {
        return symbol_string_parser_node<Symbol>(string);
    }


} //namespace parserlib


#endif // PARSERLIB_SYMBOL_STRING_PARSER_NODE_HPP
