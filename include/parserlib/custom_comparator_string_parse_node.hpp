#ifndef PARSERLIB_CUSTOM_COMPARATOR_STRING_PARSE_NODE_HPP
#define PARSERLIB_CUSTOM_COMPARATOR_STRING_PARSE_NODE_HPP


#include <type_traits>
#include <string>
#include "parse_node.hpp"
#include "parse_context_options.hpp"


namespace parserlib {


    /**
     * A string parse node that uses a custom comparator for comparisons.
     * @param Symbol string element type.
     * @param SymbolComparator the type of the symbol comparator to use.
     */
    template <class Symbol, class SymbolComparator>
    class custom_comparator_string_parse_node : public parse_node<custom_comparator_string_parse_node<Symbol, SymbolComparator>> {
    public:
        /**
         * The constructor.
         * @param string the string to parse.
         * @param the symbol comparator instance.
         */
        custom_comparator_string_parse_node(const std::basic_string<Symbol>& string, const SymbolComparator& symbol_comparator)
            : m_string(string)
            , m_symbol_comparator(symbol_comparator)
            #ifndef NDEBUG
            , m_text("custom_comparator(\"" + m_string + "\")")
            #endif            
            
        {
        }

        /**
         * Parses the string against the current input.
         * It uses the supplied symbol comparator for comparisons.
         * @param pc the current parse context.
         * @return true if the whole string is parsed successfully, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                auto thisIt = m_string.begin();
                auto parseIt = pc.parse_position().iterator();
                for (;;) {
                    if (thisIt == m_string.end()) {
                        pc.increment_parse_position(m_string.size());
                        return true;
                    }
                    if (parseIt == pc.end_iterator()) {
                        break;
                    }
                    if (m_symbol_comparator(*thisIt, *parseIt) != 0) {
                        break;
                    }
                    ++thisIt;
                    ++parseIt;
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
        const std::basic_string<Symbol> m_string;
        const SymbolComparator m_symbol_comparator;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    /**
     * Creates a string parse node out of a string and symbol comparator.
     * @param string the string to create a string parse node from.
     * @param the symbol comparator instance.
     * @return a string parse node.
     */
    template <class Symbol, class SymbolComparator>
    custom_comparator_string_parse_node<Symbol, SymbolComparator> terminal(const std::basic_string<Symbol>& string, const SymbolComparator& symbol_comparator) {
        return { string, symbol_comparator };
    }


    /**
     * Creates a parse node for a string which is case insensitive.
     * @param string the string to create a case insensitive string parse node from.
     * @return a case insensitive string parse node.
     */
    template <class Symbol>
    custom_comparator_string_parse_node<Symbol, case_insensitive_symbol_comparator> case_insensitive_terminal(const Symbol* string) {
        return { string, case_insensitive_symbol_comparator() };
    }


    /**
     * Creates a parse node for a string which does a case insensitive comparison.
     * @param string the string to create a case insensitive string parse node from.
     * @return a case insensitive string parse node.
     */
    template <class Symbol>
    custom_comparator_string_parse_node<Symbol, case_insensitive_symbol_comparator> case_insensitive_terminal(const std::basic_string<Symbol>& string) {
        return { string, case_insensitive_symbol_comparator() };
    }


} //namespace parserlib


#endif //PARSERLIB_CUSTOM_COMPARATOR_STRING_PARSE_NODE_HPP
