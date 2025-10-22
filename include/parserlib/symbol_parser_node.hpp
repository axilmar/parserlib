#ifndef PARSERLIB_SYMBOL_PARSER_NODE_HPP
#define PARSERLIB_SYMBOL_PARSER_NODE_HPP


#include <type_traits>
#include "parser_node.hpp"


namespace parserlib {


    template <class Symbol> class symbol_parser_node : public parser_node<symbol_parser_node<Symbol>> {
    public:
        symbol_parser_node(const Symbol& symbol) : m_symbol(symbol) {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            return pc.parse_symbol(m_symbol);
        }

    private:
        Symbol m_symbol;
    };


    template <class Symbol, std::enable_if_t<!std::is_base_of_v<parser_node_tag, Symbol>, bool> = true>
    symbol_parser_node<Symbol> parser(const Symbol& symbol) {
        return symbol_parser_node<Symbol>(symbol);
    }


    template <class Symbol> symbol_parser_node<Symbol> terminal(const Symbol& symbol) {
        return symbol_parser_node<Symbol>(symbol);
    }


} //namespace parserlib


#endif // PARSERLIB_SYMBOL_PARSER_NODE_HPP
