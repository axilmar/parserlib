#ifndef PARSERLIB_RANGE_PARSE_NODE_HPP
#define PARSERLIB_RANGE_PARSE_NODE_HPP


#include <cassert>
#include <sstream>
#include "parse_node.hpp"
#include "symbol_functions.hpp"


namespace parserlib {


    /**
     * A parse node which parses a symbol within a range.
     * @param Symbol symbol type.
     */
    template <class Symbol>
    class range_parse_node : public parse_node<range_parse_node<Symbol>> {
    public:
        /**
         * The constructor.
         * @param min the min value of the range.
         * @param max the max value of the range.
         */
        range_parse_node(const Symbol& min, const Symbol& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        /**
         * Parses a symbol out of a range of symbols.
         * @param pc the current parse context.
         * @return true if the current symbol to parse is within the specified range, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                const auto curr_symbol = *pc.parse_position().iterator();
                if (pc.compare_symbols(curr_symbol, m_min) >= 0 && pc.compare_symbols(curr_symbol, m_max) <= 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << symbol_text(m_min) << ".." << symbol_text(m_max);
            return stream.str();
        }

    private:
        const Symbol m_min;
        const Symbol m_max;
    };


    /**
     * Creates a range parse node out of 
     */
    template <class Symbol>
    range_parse_node<Symbol> range(const Symbol& min, const Symbol& max) {
        return range_parse_node<Symbol>(min, max);
    }


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSE_NODE_HPP
