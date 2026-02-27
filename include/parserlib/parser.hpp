#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include "parse_context.hpp"
#include "symbol_parse_node.hpp"
#include "string_parse_node.hpp"
#include "set_parse_node.hpp"
#include "range_parse_node.hpp"


namespace parserlib {


    template <class Iterator = std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parser {
    public:
        using iterator_type = Iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using symbol_comparator_type = SymbolComparator;

        using parse_context = parserlib::parse_context<Iterator, MatchId, ErrorId, SymbolComparator>;
        using parse_context_type = parse_context;

        template <class Symbol>
        static parse_node_ptr<parse_context> terminal(const Symbol& symbol) {
            return symbol;
        }

        template <class Symbol>
        static parse_node_ptr<parse_context> terminal(const Symbol* string) {
            return string;
        }

        template <class Symbol>
        static parse_node_ptr<parse_context> set(const Symbol* set) {
            return std::make_shared<set_parse_node<parse_context, Symbol>>(set);
        }

        template <class Symbol>
        static parse_node_ptr<parse_context> range(const Symbol& min, const Symbol& max) {
            return std::make_shared<range_parse_node<parse_context, Symbol>>(min, max);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
