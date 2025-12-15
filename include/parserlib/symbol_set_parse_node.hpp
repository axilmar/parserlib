#ifndef PARSERLIB_SYMBOL_SET_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_SET_PARSE_NODE_HPP


#include <string>
#include <vector>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node class that parses a symbol out of a set.
     * @param Symbol type of symbol; it must be convertible to 'int'.
     */
    template <class Symbol>
    class symbol_set_parse_node : public parse_node<symbol_set_parse_node<Symbol>> {
    public:
        /**
         * The constructor.
         * @param set the set to parse.
         */
        symbol_set_parse_node(const std::vector<Symbol>& set)
            : m_set(set)
            , m_symbol_set(get_symbol_set(set))
        {
        }

        /**
         * Parses a set.
         * @param pc the context to use for parsing.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_symbol_set(m_symbol_set);
        }

    private:
        std::vector<Symbol> m_set;
        std::vector<int> m_symbol_set;

        static std::vector<int> get_symbol_set(const std::vector<Symbol>& set) {
            std::vector<int> result;
            for (const Symbol& symbol : set) {
                result.push_back(static_cast<int>(symbol));
            }
            return result;
        }
    };


    /**
     * Creates a parse node for a set.
     * @param set set to create a set parse node of.
     * @return a set parse node.
     */
    template <class Symbol>
    symbol_set_parse_node<Symbol> set(const Symbol* set) {
       std::basic_string_view<Symbol> str_view(set);
        return std::vector<Symbol>(str_view.begin(), str_view.end());
    }


    /**
     * Creates a parse node for a set.
     * @param set set to create a set parse node of.
     * @return a set parse node.
     */
    template <class Symbol>
    symbol_set_parse_node<Symbol> set(const std::vector<Symbol>& set) {
        return set;
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_SET_PARSE_NODE_HPP
