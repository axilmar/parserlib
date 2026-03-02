#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include "parse_context.hpp"
#include "symbol_parse_node.hpp"
#include "string_parse_node.hpp"
#include "set_parse_node.hpp"
#include "range_parse_node.hpp"
#include "any_parse_node.hpp"
#include "end_parse_node.hpp"
#include "false_parse_node.hpp"
#include "true_parse_node.hpp"
#include "newline_parse_node.hpp"
#include "function_parse_node.hpp"
#include "error_parse_node.hpp"
#include "skip_before_parse_node.hpp"
#include "skip_after_parse_node.hpp"
#include "parserlib/rule.hpp"


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

        using parse_node_ptr = parserlib::parse_node_ptr<parse_context>;
        using parse_node_ptr_type = parse_node_ptr;

        using rule = parserlib::rule<parse_context>;
        using rule_type = rule;

        template <class Symbol>
        static parse_node_ptr terminal(const Symbol& symbol) {
            return symbol;
        }

        template <class Symbol>
        static parse_node_ptr terminal(const Symbol* string) {
            return string;
        }

        template <class Symbol>
        static parse_node_ptr set(const Symbol* set) {
            return std::make_shared<set_parse_node<parse_context, Symbol>>(set);
        }

        template <class Symbol>
        static parse_node_ptr range(const Symbol& min, const Symbol& max) {
            return std::make_shared<range_parse_node<parse_context, Symbol>>(min, max);
        }

        static parse_node_ptr any() {
            return std::make_shared<any_parse_node<parse_context>>();
        }

        static parse_node_ptr end() {
            return std::make_shared<end_parse_node<parse_context>>();
        }

        static parse_node_ptr false_() {
            return std::make_shared<false_parse_node<parse_context>>();
        }

        static parse_node_ptr true_() {
            return std::make_shared<true_parse_node<parse_context>>();
        }

        static parse_node_ptr newline(const parse_node_ptr& parse_node) {
            return std::make_shared<newline_parse_node<parse_context>>(parse_node);
        }

        template <class F>
        static parse_node_ptr function(const F& func) {
            return std::make_shared<function_parse_node<parse_context, F>>(func);
        }

        static parse_node_ptr error(const ErrorId& id, const parse_node_ptr& parse_node) {
            return std::make_shared<error_parse_node<parse_context>>(parse_node, id);
        }

        static parse_node_ptr skip_before(const parse_node_ptr& valid_parse_node, const parse_node_ptr& invalid_parse_node = {}) {
            return std::make_shared<skip_before_parse_node<parse_context>>(valid_parse_node, invalid_parse_node);
        }

        static parse_node_ptr skip_after(const parse_node_ptr& valid_parse_node, const parse_node_ptr& invalid_parse_node = {}) {
            return std::make_shared<skip_after_parse_node<parse_context>>(valid_parse_node, invalid_parse_node);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
