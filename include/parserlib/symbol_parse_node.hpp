#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node class that parses a single symbol.
     * @param Symbol Type of symbol to parse; must be convertible to 'int'.
     */
    template <class Symbol>
    class symbol_parse_node : public parse_node<symbol_parse_node<Symbol>> {
    public:
        /**
         * The constructor.
         * @param symbol the symbol to parse.
         */
        symbol_parse_node(const Symbol& symbol) 
            : m_symbol(symbol)
        {
        }

        /**
         * Parses a symbol.
         * @param pc the context to use for parsing.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_symbol(static_cast<int>(m_symbol));
        }

    private:
        Symbol m_symbol;
    };


    /**
     * Creates a symbol parse node.
     * @param symbol the symbol to parse.
     * @return a symbol parse node.
     */
    template <class Symbol>
    symbol_parse_node<Symbol> terminal(const Symbol& symbol) {
        return symbol;
    }


    /**
     * Creates a symbol parse node.
     * @param symbol the symbol to parse; must not be a boolean.
     * @return a symbol parse node.
     */
    template <class Symbol, std::enable_if_t<!std::is_same_v<Symbol, bool> && !std::is_base_of_v<parse_node_tag, Symbol>, bool> = true>
    symbol_parse_node<Symbol> make_parse_node(const Symbol& symbol) {
        return symbol;
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
