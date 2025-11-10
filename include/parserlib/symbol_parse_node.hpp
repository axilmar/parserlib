#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"
#include "is_parse_functor.hpp"


namespace parserlib {


    /**
     * A parse node that parses a specific symbol.
     * @param Symbol type of symbol to parse.
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
         * Parses a specific symbol.
         * @param pc the current parse context to use for parsing.
         * @return true if the current symbol from source equals the given symbol,
         *  false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                if (pc.compare_current_symbol(m_symbol) == 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

    private:
        const Symbol m_symbol;
    };


    /**
     * Creates a parse node out of the given symbol.
     * @param symbol the parse node to create a symbol parse node from.
     * @return the symbol parse node.
     */
    template <class Symbol, std::enable_if_t<!std::is_base_of_v<parse_node_base, Symbol> && !std::is_same_v<Symbol, bool> && !is_parse_functor_v<Symbol>, bool> = true>
    symbol_parse_node<Symbol> make_parse_node(const Symbol& symbol) {
        return symbol_parse_node<Symbol>(symbol);
    }


    /**
     * Creates a parse node out of the given symbol.
     * @param symbol the parse node to create a symbol parse node from.
     * @return the symbol parse node.
     */
    template <class Symbol>
    symbol_parse_node<Symbol> terminal(const Symbol& symbol) {
        return make_parse_node(symbol);
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
