#ifndef PARSERLIB_SYMBOL_RANGE_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_RANGE_PARSE_NODE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that parses a symbol within a range.
     */
    template <class Symbol>
    class symbol_range_parse_node : public parse_node<symbol_range_parse_node<Symbol>> {
    public:
        symbol_range_parse_node(const Symbol& min, const Symbol& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_symbol_range(static_cast<int>(m_min), static_cast<int>(m_max));
        }

    private:
        Symbol m_min;
        Symbol m_max;
    };


    template <class Symbol>
    symbol_range_parse_node<Symbol> range(const Symbol& min, const Symbol& max) {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_RANGE_PARSE_NODE_HPP
