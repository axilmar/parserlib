#ifndef PARSERLIB_CUSTOM_COMPARATOR_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_CUSTOM_COMPARATOR_SYMBOL_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"
#include "is_parse_functor.hpp"
#include "parse_context_options.hpp"
#include "symbol_functions.hpp"


namespace parserlib {


    /**
     * A Symbol parse node that uses a custom comparator for comparisons.
     * @param Symbol type of symbol to parse.
     * @param SymbolComparator the type of the symbol comparator to use.
     */
    template <class Symbol, class SymbolComparator>
    class custom_comparator_symbol_parse_node : public parse_node<custom_comparator_symbol_parse_node<Symbol, SymbolComparator>> {
    public:
        /**
         * The constructor.
         * @param symbol the symbol to parse.
         * @param symbol_comparator the symbol comparator to use.
         */
        custom_comparator_symbol_parse_node(const Symbol& symbol, const SymbolComparator& symbol_comparator)
            : m_symbol(symbol)
            , m_symbol_comparator(symbol_comparator)
            #ifndef NDEBUG
            , m_text("custom_comparator(" + symbol_text(m_symbol) + ")")
            #endif                        
        {
        }

        /**
         * Parses a specific symbol, using the supplied comparator.
         * @param pc the current parse context to use for parsing.
         * @return true if the current symbol from source equals the given symbol,
         *  false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                if (m_symbol_comparator(*pc.parse_position().iterator(), m_symbol) == 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        #ifndef NDEBUG
        std::string text() const {
            return m_text;
        }
        #endif

    private:
        const Symbol m_symbol;
        const SymbolComparator m_symbol_comparator;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    /**
     * Creates a parse node out of the given symbol and symbol comparator.
     * @param symbol the symbol to create a symbol parse node from.
     * @param symbol_comparator the symbol comparator to use.
     * @return the symbol parse node.
     */
    template <class Symbol, class SymbolComparator>
    custom_comparator_symbol_parse_node<Symbol, SymbolComparator> terminal(const Symbol& symbol, const SymbolComparator& symbol_comparator) {
        return { symbol, symbol_comparator };
    }


    /**
     * Creates a parse node for a symbol which does a case insensitive comparison.
     * @param symbol the symbol to create a symbol parse node from.
     * @return a case insensitive string parse node.
     */
    template <class Symbol>
    custom_comparator_symbol_parse_node<Symbol, case_insensitive_symbol_comparator> case_insensitive_terminal(const Symbol& symbol) {
        return { symbol, case_insensitive_symbol_comparator() };
    }


} //namespace parserlib


#endif //PARSERLIB_CUSTOM_COMPARATOR_SYMBOL_PARSE_NODE_HPP
