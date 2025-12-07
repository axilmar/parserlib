#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include "container_position.hpp"
#include "non_converting_symbol_comparator.hpp"
#include "parse_position.hpp"
#include "match.hpp"
#include "error.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class Source = std::string, class MatchId = int, class ErrorId = int, class SourcePosition = container_position, class SymbolComparator = non_converting_symbol_comparator>
    class parse_context {
    public:
        using source_type = Source;
        using iterator_type = typename Source::const_iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using source_position_type = SourcePosition;
        using symbol_comparator_type = SymbolComparator;
        using parse_context_type = parse_context<Source, MatchId, ErrorId, SourcePosition, SymbolComparator>;
        using parse_position_type = parse_position<iterator_type, SourcePosition>;
        using match_type = match<MatchId, parse_position_type>;
        using match_container_type = typename match_type::match_container_type;
        using error_type = error_<ErrorId, parse_position_type>;
        using error_container_type = std::vector<error_type>;
        using rule_type = rule<parse_context_type>;

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state{ begin, {begin, 0}, end }
            , m_errors(1)
            , m_initial_rule_left_recursion_state{ begin, rule_left_recursion_status::none }
            , m_end(end)
        {
        }

        parse_context(Source& source)
            : parse_context(source.begin(), source.end())
        {
        }

        parse_position_type get_parse_position() const {
            return m_state.m_parse_position;
        }

        bool is_valid_parse_position() const {
            return m_state.m_parse_position.get_iterator() != m_end;
        }

        bool is_end_parse_position() const {
            return m_state.m_parse_position.get_iterator() == m_end;
        }

        const match_container_type& get_matches() const {
            return m_matches;
        }

        const error_container_type& get_errors() const {
            return m_errors.front();
        }

        template <class Symbol>
        bool parse_symbol(const Symbol& symbol) {
        }

        template <class Container>
        bool parse_string(const Container& container) {
        }

        template <class Container>
        bool parse_set(const Container& container) {
        }

        template <class Symbol>
        bool parse_range(const Symbol& min, const Symbol& max) {
        }

        bool parse_any() {
        }

        bool parse_end() {
        }

        template <class ParseNode>
        bool parse_loop_0(const ParseNode& parse_node) {
        }

        template <class ParseNode>
        bool parse_loop_1(const ParseNode& parse_node) {
        }

        template <class ParseNode>
        bool parse_optional(const ParseNode& parse_node) {
        }

        template <class ParseNode>
        bool parse_logical_and(const ParseNode& parse_node) {
        }

        template <class ParseNode>
        bool parse_logical_not(const ParseNode& parse_node) {
        }

        template <class ParseNodes>
        bool parse_sequence(const ParseNodes& parse_nodes) {
        }

        template <class ParseNodes>
        bool parse_choice(const ParseNodes& parse_nodes) {
        }

        template <class ParseNode>
        bool parse_match(const MatchId& id, const ParseNode& parse_node) {
        }

        template <class SkipParseNode>
        bool parse_error_skip_before(const ErrorId& id, const SkipParseNode& skip_parse_node) {
        }

        template <class SkipParseNode>
        bool parse_error_skip_after(const ErrorId& id, const SkipParseNode& skip_parse_node) {
        }

    private:
        struct match_start_state {
            parse_position_type m_parse_position;
            size_t m_match_count;
        };

        struct state {
            parse_position_type m_parse_position;
            match_start_state m_match_start_state;
            iterator_type m_end;
        };

        enum class rule_left_recursion_status {
            none,
            reject,
            accept
        };

        struct rule_left_recursion_state {
            iterator_type m_iterator;
            rule_left_recursion_status m_status;
        };

        state m_state;
        match_container_type m_matches;
        std::vector<error_container_type> m_errors;
        std::map<rule_type*, rule_left_recursion_state> m_rule_left_recursion_state;
        const SymbolComparator m_symbol_comparator;
        const rule_left_recursion_state m_initial_rule_left_recursion_state;
        const iterator_type m_end;

        /*
        void update_state_match_start_state() {
            m_state.m_match_start_state = match_start_state(m_state.m_parse_position, m_matches.size());
        }
        */
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
