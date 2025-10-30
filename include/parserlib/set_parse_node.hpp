#ifndef PARSERLIB_SET_PARSE_NODE_HPP
#define PARSERLIB_SET_PARSE_NODE_HPP


#include <string>
#include <algorithm>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node which parses a symbol out of a set of symbols.
     * @param Symbol symbol type.
     */
    template <class Symbol>
    class set_parse_node : public parse_node<set_parse_node<Symbol>> {
    public:
        /**
         * The constructor.
         * @param set symbol set.
         */
        set_parse_node(const std::basic_string<Symbol>& set) 
            : m_set(sort_set(set))
        {
        }

        /**
         * Parses the current symbol out of the set stored internally.
         * @param pc the current parse context.
         * @return true if the current symbol to parse is contained in the set, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                const auto curr_symbol = *pc.parse_position().iterator();
                auto it = std::upper_bound(m_set.begin(), m_set.end(), curr_symbol, [&](const auto& curr_symbol, const auto& set_symbol) {
                    return pc.compare_symbols(curr_symbol, set_symbol) < 0;
                });
                if (it != m_set.begin()) {
                    --it;
                    if (pc.compare_symbols(curr_symbol, *it) == 0) {
                        pc.increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

    private:
        const std::basic_string<Symbol> m_set;

        static std::basic_string<Symbol> sort_set(std::basic_string<Symbol> set) {
            std::sort(set.begin(), set.end());
            return set;
        }
    };


    /**
     * Creates a symbol set parse node from a null-terminated string.
     * @param the null-terminated string to create a set parse node from.
     * @return a set parse node.
     */
    template <class Symbol>
    set_parse_node<Symbol> set(const Symbol* string) {
        return set_parse_node<Symbol>(string);
    }


    /**
     * Creates a symbol set parse node from a string.
     * @param the null-terminated string to create a set parse node from.
     * @return a set parse node.
     */
    template <class Symbol>
    set_parse_node<Symbol> set(const std::basic_string<Symbol>& string) {
        return set_parse_node<Symbol>(string);
    }


} //namespace parserlib


#endif //PARSERLIB_SET_PARSE_NODE_HPP
