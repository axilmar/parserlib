#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <type_traits>
#include <functional>
#include "container_position.hpp"
#include "non_converting_symbol_comparator.hpp"
#include "parse_position.hpp"
#include "match.hpp"
#include "error.hpp"
#include "rule_left_recursion_exception.hpp"


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
        using parse_function_type = std::function<bool(parse_context_type&)>;
        using rule_left_recursion_exception_type = rule_left_recursion_exception<parse_context_type>;

    private:
        struct _match_start_state {
            parse_position_type m_parse_position;
            size_t m_match_count;
        };

    public:
        struct state {
        private:
            parse_position_type m_parse_position;
            _match_start_state m_match_start_state;
            iterator_type m_end;

            state(const iterator_type& begin, const iterator_type& end)
                : m_parse_position(begin)
                , m_match_start_state{ begin, 0 }
                , m_end(end)
            {
            }

            friend parse_context_type;
        };

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state{ begin, end }
            , m_first_unparsed_position{ begin }
            , m_errors(1)
            , m_initial_rule_left_recursion_state{ begin, _rule_left_recursion_status::none }
            , m_end(end)
        {
        }

        parse_context(Source& source)
            : parse_context(source.begin(), source.end())
        {
        }

        const state& get_state() const {
            return m_state;
        }

        const parse_position_type& get_parse_position() const {
            return m_state.m_parse_position;
        }

        bool is_valid_parse_position() const {
            return m_state.m_parse_position.get_iterator() != m_state.m_end;
        }

        bool is_end_parse_position() const {
            return !is_valid_parse_position();
        }

        const parse_position_type& get_first_unparsed_position() const {
            return m_first_unparsed_position;
        }

        const match_container_type& get_matches() const {
            return m_matches;
        }

        const error_container_type& get_errors() const {
            return m_errors.front();
        }

        auto get_current_symbol() const {
            return *m_state.m_parse_position.get_iterator();
        }

        template <class A, class B>
        int compare_symbols(const A& a, const B& b) const {
            return m_symbol_comparator(a, b);
        }

        template <class T>
        int compare_current_symbol(const T& symbol) const {
            return compare_symbols(get_current_symbol(), symbol);
        }

        void set_state(const state& st) {
            m_state = st;
            m_matches.resize(st.m_match_start_state.m_match_count);
        }

        void set_parse_position(const parse_position_type& pp) {
            m_state.m_parse_position = pp;
        }

        parse_context_type& operator ++() {
            ++m_state.m_parse_position;
            _update_first_unparsed_position();
            _update_match_start_state();
            return *this;
        }

        parse_context_type& operator +=(size_t count) {
            m_state.m_parse_position += count;
            _update_first_unparsed_position();
            _update_match_start_state();
            return *this;
        }

        void increment_line() {
            m_state.m_parse_position.increment_line();
            m_state.m_match_start_state.m_parse_position.set_source_position(m_state.m_parse_position.get_source_position());
        }

        void add_match(const MatchId& id, const state& begin_state, const parse_position_type& end_parse_position) {
            _add_match(id, begin_state.m_match_start_state, end_parse_position);
        }

        void add_error(const ErrorId& id, const parse_position_type& begin_parse_position, const parse_position_type& end_parse_position) {
            m_errors.back().emplace_back(id, begin_parse_position, end_parse_position);
        }

        template <class Symbol>
        bool parse_symbol(const Symbol& symbol) {
            if (is_valid_parse_position()) {
                if (compare_current_symbol(symbol) == 0) {
                    operator ++();
                    return true;
                }
            }
            return false;
        }

        template <class Container>
        bool parse_string(const Container& container) {
            if (is_valid_parse_position()) {
                const auto itContainer = container.begin();
                const auto itParseContext = m_state.m_parse_position.get_iterator();
                for (;;) {
                    if (itContainer == container.end()) {
                        operator +=(container.size());
                        return true;
                    }
                    if (itParseContext == m_state.m_end || compare_symbols(*itParseContext, *itContainer) != 0) {
                        return false;
                    }
                    ++itContainer;
                    ++itParseContext;
                }
            }
            return false;
        }

        template <class Container>
        bool parse_set(const Container& container) {
            if (is_valid_parse_position()) {
                const auto currSymbol = get_current_symbol();
                for (const auto& setSymbol : container) {
                    if (compare_symbols(currSymbol, setSymbol) == 0) {
                        return true;
                    }
                }
            }
            return false;
        }

        template <class Symbol>
        bool parse_range(const Symbol& min, const Symbol& max) {
            assert(min <= max);
            if (is_valid_parse_position()) {
                const auto currSymbol = get_current_symbol();
                if (compare_symbols(currSymbol, min) >= 0 && compare_symbols(currSymbol, max) <= 0) {
                    operator ++();
                    return true;
                }
            }
            return false;
        }

        bool parse_any() {
            if (is_valid_parse_position()) {
                operator ++();
                return true;
            }
            return false;
        }

        bool parse_end() {
            return is_end_parse_position();
        }

        template <class ParseNode>
        bool parse_newline(const ParseNode& parse_node) {
            if (is_valid_parse_position()) {
                if (parse_node.parse(*this)) {
                    increment_line();
                    return true;
                }
            }
            return false;
        }

        template <class ParseNode>
        bool parse_loop0(const ParseNode& parse_node) {
            while (parse_node.parse(*this)) {                
            }
            return true;
        }

        template <class ParseNode>
        bool parse_loop1(const ParseNode& parse_node) {
            if (parse_node.parse(*this)) {
                return parse_loop_0(parse_node);
            }
            return false;
        }

        template <class ParseNode>
        bool parse_optional(const ParseNode& parse_node) {
            parse_node.parse(*this);
            return true;
        }

        template <class ParseNode>
        bool parse_logical_and(const ParseNode& parse_node) {
            const state prev_state = m_state;
            try {
                bool result = parse_node.parse(*this);
                set_state(prev_state);
                return result;
            }
            catch (...) {
                set_state(prev_state);
                throw;
            }
        }

        template <class ParseNode>
        bool parse_logical_not(const ParseNode& parse_node) {
            const state prev_state = m_state;
            try {
                bool result = !parse_node.parse(*this);
                set_state(prev_state);
                return result;
            }
            catch (...) {
                set_state(prev_state);
                throw;
            }
        }

        template <class ParseNodes>
        bool parse_sequence(const ParseNodes& parse_nodes) {
            return _parse_sequence<0>(parse_nodes);
        }

        template <class ParseNodes>
        bool parse_choice(const ParseNodes& parse_nodes) {
            return _parse_choice(parse_nodes);
        }

        template <class ParseNode>
        bool parse_match(const MatchId& id, const ParseNode& parse_node) {
            const _match_start_state mss = m_state.m_match_start_state;
            if (parse_node.parse(*this)) {
                _add_match(id, mss, m_state.m_parse_position);
                return true;
            }
            return false;
        }

        template <class ParseNode>
        bool parse_error_skip_before(const ErrorId& id, const ParseNode& test_parse_node) {
            if (is_valid_parse_position()) {
                const state initial_state = m_state;
                do {
                    const state prev_state = m_state;
                    try {
                        const bool result = test_parse_node.parse(*this);
                        set_state(prev_state);
                        if (result) {
                            break;
                        }
                    }
                    catch (...) {
                        set_state(initial_state);
                        throw;
                    }
                    operator ++();
                } while (is_valid_parse_position());
                add_error(id, initial_state.m_parse_position, m_state.m_parse_position);
                return true;
            }
            return false;
        }

        template <class ParseNode>
        bool parse_error_skip_after(const ErrorId& id, const ParseNode& test_parse_node) {
            if (is_valid_parse_position()) {
                const state initial_state = m_state;
                do {
                    const state prev_state = m_state;
                    try {
                        if (test_parse_node.parse(*this)) {
                            break;
                        }
                    }
                    catch (...) {
                        set_state(initial_state);
                        throw;
                    }
                    set_state(prev_state);
                    operator ++();
                } while (is_valid_parse_position());
                add_error(id, initial_state.m_parse_position, m_state.m_parse_position);
                return true;
            }
            return false;
        }

        bool parse_rule(rule_type& rule, const parse_function_type& fn) {
            _rule_left_recursion_state& rule_state = _get_rule_left_recursion_state(rule);
            if (m_state.m_parse_position.get_iterator() != rule_state.m_iterator) {
                return _parse_rule(rule, fn, rule_state);
            }
            else {
                switch (rule_state.m_status) {
                    case _rule_left_recursion_status::none:
                        throw rule_left_recursion_exception_type(rule);

                    case _rule_left_recursion_status::reject:
                        return false;

                    case _rule_left_recursion_status::accept:
                        _unlock_parsing();
                        return true;
                }
            }
            throw std::runtime_error("parserlib::parse_context::parse_rule: invalid rule state.");
        }

    private:
        enum class _rule_left_recursion_status {
            none,
            reject,
            accept
        };

        struct _rule_left_recursion_state {
            iterator_type m_iterator;
            _rule_left_recursion_status m_status;
        };

        state m_state;
        parse_position_type m_first_unparsed_position;
        match_container_type m_matches;
        std::vector<error_container_type> m_errors;
        std::map<rule_type*, _rule_left_recursion_state> m_rule_left_recursion_state;
        const SymbolComparator m_symbol_comparator;
        const _rule_left_recursion_state m_initial_rule_left_recursion_state;
        const iterator_type m_end;

        void _update_first_unparsed_position() {
            if (m_state.m_parse_position > m_first_unparsed_position) {
                m_first_unparsed_position = m_state.m_parse_position;
            }
        }

        void _update_match_start_state() {
            m_state.m_match_start_state = _match_start_state{ m_state.m_parse_position, m_matches.size() };
        }

        template <size_t Index, class ParseNodes>
        bool _parse_sequence(const ParseNodes& parse_nodes) {
            if constexpr (Index < std::tuple_size_v<ParseNodes>) {
                const auto& parse_node = std::get<Index>(parse_nodes);
                if (parse_node.parse(*this)) {
                    return _parse_sequence<Index + 1>(parse_nodes);
                }
                return false;
            }
            else {
                return true;
            }
        }

        void _integrate_curr_errors(error_container_type& choice_errors) {
            error_container_type& curr_errors = m_errors.back();
            if (curr_errors.empty()) {
                return;
            }
            if (choice_errors.empty() || curr_errors.back().get_begin_parse_position() > choice_errors.back().get_begin_parse_position()) {
                choice_errors = std::move(curr_errors);
            }
            else {
                curr_errors.clear();
            }
        }

        template <class ParseNode> 
        bool _parse_choice_branch(const ParseNode& parse_node, error_container_type& choice_errors) {
            const state prev_state = m_state;
            try {
                if (parse_node.parse(*this)) {
                    return true;
                }
                set_state(prev_state);
                _integrate_curr_errors(choice_errors);
                return false;
            }
            catch (...) {
                set_state(prev_state);
                throw;
            }
        }

        template <size_t Index, class ParseNodes>
        bool _parse_choice_helper(const ParseNodes& parse_nodes, error_container_type& choice_errors) {
            if constexpr (Index < std::tuple_size_v<ParseNodes>) {
                const auto& parse_node = std::get<Index>(parse_nodes);
                if (_parse_choice_branch(parse_node, choice_errors)) {
                    return true;
                }
                return _parse_choice_helper<Index + 1>(parse_nodes, choice_errors);
            }
            else {
                return false;
            }
        }

        template <class ParseNodes>
        bool _parse_choice(const ParseNodes& parse_nodes) {
            m_errors.emplace_back();
            error_container_type choice_errors;
            try {
                if (_parse_choice_helper<0>(parse_nodes, choice_errors)) {
                    m_errors.pop_back();
                    return true;
                }
                m_errors.pop_back();
                m_errors.back().insert(m_errors.back().end(), choice_errors.begin(), choice_errors.end());
                return false;
            }
            catch (...) {
                m_errors.pop_back();
                throw;
            }
        }

        void _add_match(const MatchId& id, const _match_start_state& mss, const parse_position_type& end_parse_position) {
            assert(mss.m_match_count <= m_matches.size());
            match_container_type children(std::make_move_iterator(m_matches.begin() + mss.m_match_count), std::make_move_iterator(m_matches.end()));
            m_matches.resize(mss.m_match_count);
            m_matches.emplace_back(id, mss.m_parse_position, end_parse_position, std::move(children));
            m_state.m_match_start_state.m_match_count = m_matches.size();
        }

        _rule_left_recursion_state& _get_rule_left_recursion_state(rule_type& rule) {
            const auto [it, ok] = m_rule_left_recursion_state.insert(std::make_pair(std::addressof(rule), m_initial_rule_left_recursion_state));
            return it->second;
        }

        bool _parse_rule(rule_type& rule, const parse_function_type& fn, _rule_left_recursion_state& rule_state) {
            _rule_left_recursion_state prev_rule_state = rule_state;
            try {
                rule_state.m_iterator = m_state.m_parse_position.get_iterator();
                rule_state.m_status = _rule_left_recursion_status::none;
                const bool result = fn(*this);
                rule_state = prev_rule_state;
                return result;
            }
            catch (const rule_left_recursion_exception_type& ex) {
                return _handle_left_recursion_exception(rule, fn, rule_state, ex, prev_rule_state);
            }
            catch (...) {
                rule_state = prev_rule_state;
                throw;
            }
        }

        bool _handle_left_recursion_exception(rule_type& rule, const parse_function_type& fn, _rule_left_recursion_state& rule_state, const rule_left_recursion_exception_type& ex, const _rule_left_recursion_state& prev_rule_state) {
            if (std::addressof(ex.get_rule()) == std::addressof(rule)) {
                try {
                    const bool result = _handle_left_recursion(rule, fn, rule_state);
                    rule_state = prev_rule_state;
                    return result;
                }
                catch (...) {
                    rule_state = prev_rule_state;
                    throw;
                }
            }
            rule_state = prev_rule_state;
            throw ex;
        }

        bool _handle_left_recursion(rule_type& rule, const parse_function_type& fn, _rule_left_recursion_state& rule_state) {
            const _match_start_state mss = m_state.m_match_start_state;
            if (_handle_left_recursion_reject(rule, fn, rule_state)) {
                return _handle_left_recursion_accept(rule, fn, rule_state, mss);
            }
            return false;
        }

        bool _handle_left_recursion_reject(rule_type& rule, const parse_function_type& fn, _rule_left_recursion_state& rule_state) {
            rule_state.m_status = _rule_left_recursion_status::reject;
            return fn(*this);
        }

        bool _handle_left_recursion_accept(rule_type& rule, const parse_function_type& fn, _rule_left_recursion_state& rule_state, const _match_start_state& mss) {
            rule_state.m_status = _rule_left_recursion_status::accept;
            for(;;) {
                m_state.m_match_start_state = mss;
                _lock_parsing();
                rule_state.m_iterator = m_state.m_parse_position.get_iterator();
                if (!fn(*this)) {
                    break;
                }
            }
            _unlock_parsing();
            return true;
        }

        void _lock_parsing() {
            m_state.m_end = m_state.m_parse_position.get_iterator();
        }

        void _unlock_parsing() {
            m_state.m_end = m_end;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
