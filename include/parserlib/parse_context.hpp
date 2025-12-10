#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <string>
#include <cctype>
#include <map>
#include "interface_parse_context.hpp"
#include "match.hpp"
#include "error.hpp"


namespace parserlib {


    class container_position {
    public:
        size_t get_index() const {
            return m_index;
        }

        bool operator == (const container_position& other) const {
            return m_index == other.m_index;
        }

        bool operator != (const container_position& other) const {
            return m_index != other.m_index;
        }

        bool operator < (const container_position& other) const {
            return m_index < other.m_index;
        }

        bool operator <= (const container_position& other) const {
            return m_index <= other.m_index;
        }

        bool operator > (const container_position& other) const {
            return m_index > other.m_index;
        }

        bool operator >= (const container_position& other) const {
            return m_index >= other.m_index;
        }

        void increment() {
            ++m_index;
        }

        void increment_line() {
        }

    private:
        size_t m_index{ 0 };
    };


    class file_position {
    public:
        size_t get_column() const {
            return m_column;
        }

        size_t get_line() const {
            return m_line;
        }

        bool operator == (const file_position& other) const {
            return m_line == other.m_line && m_column == other.m_column;
        }

        bool operator != (const file_position& other) const {
            return m_line != other.m_line && m_column != other.m_column;
        }

        bool operator < (const file_position& other) const {
            return m_line < other.m_line || (m_line == other.m_line && m_column < other.m_column);
        }

        bool operator <= (const file_position& other) const {
            return m_line < other.m_line || (m_line == other.m_line && m_column <= other.m_column);
        }

        bool operator > (const file_position& other) const {
            return m_line > other.m_line || (m_line == other.m_line && m_column > other.m_column);
        }

        bool operator >= (const file_position& other) const {
            return m_line > other.m_line || (m_line == other.m_line && m_column >= other.m_column);
        }

        void increment() {
            ++m_column;
        }

        void increment_line() {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_column{ 1 };
        size_t m_line{ 1 };
    };


    class case_sensitive_symbol_comparator {
    public:
        template <class A, class B>
        int operator ()(const A& a, const B& b) const {
            return static_cast<int>(a) - static_cast<int>(b);
        }
    };


    class case_insensitive_symbol_comparator {
    public:
        template <class A, class B>
        int operator ()(const A& a, const B& b) const {
            return std::tolower(static_cast<int>(a)) - std::tolower(static_cast<int>(b));
        }
    };


    template <
        class Source = std::string, 
        class MatchId = int, 
        class ErrorId = int, 
        class SourcePosition = container_position, 
        class SymbolComparator = case_sensitive_symbol_comparator
    > 
    class parse_context : public interface::parse_context {
    public:
        // types

        using source_type = Source;
        using iterator_type = typename Source::const_iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using source_position_type = SourcePosition;
        using symbol_comparator_type = SymbolComparator;
        using parse_position_type = parse_position<source_type, source_position_type>;
        using match_type = match<match_id_type, parse_position_type>;
        using match_container_type = std::vector<match_type>;
        using error_type = error_<error_id_type, parse_position_type>;
        using error_container_type = std::vector<error_type>;        

        // constructors

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_active_state{ get_initial_state(begin, end) }
            , m_end(end)
            , m_initial_rule_state_vector{ get_initial_rule_state(end) }
        {
        }

        parse_context(Source& source)
            : parse_context(source.begin(), source.end())
        {
        }

        // parse position API

        bool is_valid_parse_position() const override {
            return m_active_state.parse_position.get_iterator() != m_active_state.end;
        }

        bool is_end_parse_position() const override {
            return m_active_state.parse_position.get_iterator() == m_active_state.end;
        }

        void increment_parse_position() override {
            m_active_state.parse_position.increment();
            m_active_state.match_start_state.parse_position = m_active_state.parse_position;
        }

        void increment_parse_position_line() override {
            m_active_state.parse_position.increment_line();
            m_active_state.match_start_state.parse_position = m_active_state.parse_position;
        }

        // symbol API

        int get_current_symbol() const override {
            return static_cast<int>(*m_active_state.parse_position.get_iterator());
        }

        int compare_symbols(int a, int b) const override {
            return m_symbol_comparator(a, b);
        }

        // state API

        void push_state() override {
            m_state_stack.push_back(m_active_state);
        }

        void pop_state() override {
            m_active_state = m_state_stack.back();
            m_state_stack.pop_back();
        }

        void pop_state_without_activation() override {
            m_state_stack.pop_back();
        }

        // match API

        void push_match_start_state() override {
            m_match_start_state_stack.emplace_back(m_active_state.match_start_state);
        }

        void pop_match_start_state() override {
            m_active_state.match_start_state = m_match_start_state_stack.back();
            m_match_start_state_stack.pop_back();
        }

        void add_match(int id) override {
            assert(!m_match_start_state_stack.empty());
            const match_start_state& last_match_start_state = m_match_start_state_stack.back();
            match_container_type children(m_matches.begin() + last_match_start_state.match_count, m_matches.end());
            m_matches.resize(last_match_start_state.match_count);
            m_matches.emplace_back(static_cast<match_id_type>(id), last_match_start_state.parse_position, m_active_state.parse_position, std::move(children));
        }

        // error API
        
        void push_error_start_parse_position() override {
            m_error_start_parse_position_stack.push_back(m_active_state.parse_position);
        }

        void pop_error_start_parse_position() override {
            m_error_start_parse_position_stack.pop_back();
        }

        void add_error(int id) override {
            assert(!m_error_start_parse_position_stack.empty());
            m_errors.emplace_back(static_cast<error_id_type>(id), m_error_start_parse_position_stack.back(), m_active_state.parse_position);
        }

        // left recursion API

        bool is_rule_left_recursive_at_current_parse_position(rule& r) const override {
            return get_rule_state(r).iterator == m_active_state.parse_position.get_iterator();
        }

        rule_status get_rule_status(rule& r) const override {
            return get_rule_state(r).status;
        }

        void push_rule_state(rule& r, rule_status rs) override {
            get_rule_state_container(r).push_back(rule_state{ m_active_state.parse_position.get_iterator(), rs });
        }

        void pop_rule_state(rule& r) override {
            get_rule_state_container(r).pop_back();
        }

        void begin_accept_left_recursion() override {
            m_active_state.end = m_active_state.parse_position.get_iterator();
        }

        void end_accept_left_recursion() override {
            m_active_state.end = m_end;
        }

        // getters

        const parse_position_type& get_parse_position() const {
            return m_active_state.parse_position;
        }

        const match_container_type& get_matches() const {
            return m_matches;
        }

        const error_container_type& get_errors() const {
            return m_errors;
        }

    private:
        struct match_start_state {
            parse_position_type parse_position;
            size_t match_count;
        };

        struct state {
            parse_position_type parse_position;
            struct match_start_state match_start_state;
            iterator_type end;
        };

        struct rule_state {
            iterator_type iterator;
            rule_status status;
        };

        //state
        std::vector<state> m_state_stack;
        state m_active_state;

        //matches
        std::vector<match_start_state> m_match_start_state_stack;
        match_container_type m_matches;

        //errors
        std::vector<parse_position_type> m_error_start_parse_position_stack;
        error_container_type m_errors;

        //rules
        using rule_state_container_type = std::vector<rule_state>;
        mutable std::map<rule*, rule_state_container_type> m_rule_state;

        //constants
        const symbol_comparator_type m_symbol_comparator;
        const iterator_type m_end;
        const rule_state_container_type m_initial_rule_state_vector;

        static parse_position_type get_initial_parse_position(const iterator_type& begin) {
            return parse_position_type(begin, source_position_type());
        }

        static match_start_state get_initial_match_start_state(const iterator_type& begin) {
            return match_start_state{ get_initial_parse_position(begin), 0 };
        }

        static rule_state get_initial_rule_state(const iterator_type& begin) {
            return rule_state{ begin, rule_status::none };
        }

        static state get_initial_state(const iterator_type& begin, const iterator_type& end) {
            return state{ get_initial_parse_position(begin), get_initial_match_start_state(begin), end };
        }

        rule_state_container_type& get_rule_state_container(rule& r) const {
            const auto [it, ok] = m_rule_state.insert(std::make_pair(std::addressof(r), m_initial_rule_state_vector));
            return it->second;
        }

        rule_state& get_rule_state(rule& r) const {
            return get_rule_state_container(r).back();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
