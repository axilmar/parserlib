#ifndef PARSERLIB_SYMBOL_RANGE_PARSER_NODE_HPP
#define PARSERLIB_SYMBOL_RANGE_PARSER_NODE_HPP


#include <cassert>
#include "parser_node.hpp"


namespace parserlib {



    template <class Symbol> class symbol_range_parser_node : public parser_node<symbol_range_parser_node<Symbol>> {
    public:
        symbol_range_parser_node(const Symbol& min, const Symbol& max) : m_min(min), m_max(max) {
            assert(min <= max);
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            return pc.parse_symbol_range(m_min, m_max);
        }

    private:
        Symbol m_min;
        Symbol m_max;
    };



    template <class Symbol> symbol_range_parser_node<Symbol> range(const Symbol& min, const Symbol& max) {
        return symbol_range_parser_node<Symbol>(min, max);
    }


} //namespace parserlib


#endif // PARSERLIB_SYMBOL_RANGE_PARSER_NODE_HPP
