#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <string>
#include <cctype>
#include <sstream>
#include <map>
#include <stdexcept>
#include "parse_context_interface.hpp"
#include "parse_match.hpp"
#include "parse_error.hpp"


namespace parserlib {


    class empty_text_position {
    public:
        size_t line() const {
            return 0;
        }

        size_t column() const {
            return 0;
        }

        void increment_column() {
        }

        void increment_line() {
        }
    };


    class text_position {
    public:
        size_t line() const {
            return m_line;
        }

        size_t column() const {
            return m_column;
        }

        void increment_column() {
            ++m_column;
        }

        void increment_line() {
            ++m_line;
            m_column = 1;
        }

        std::string to_string() const {
            std::stringstream stream;
            stream << "line " << m_line << ", column " << m_column;
            return stream.str();
        }

    private:
        size_t m_line{ 1 };
        size_t m_column{ 1 };
    };


    class case_sensitive_symbol_comparator {
    public:
        int operator ()(int a, int b) const {
            return a - b;
        }
    };


    class case_insensitive_symbol_comparator {
    public:
        int operator ()(int a, int b) const {
            return std::tolower(a) - std::tolower(b);
        }
    };


    class left_recursion_exception {
    public:
        left_recursion_exception(rule* r)
            : m_rule(r)
        {
        }

        class rule* rule() const {
            return m_rule;
        }

    private:
        class rule* m_rule;
    };


    template <class Source = std::string, class MatchId = int, class ErrorId = int, class TextPosition = empty_text_position, class SymbolComparator = case_sensitive_symbol_comparator>
    class parse_context : public parse_context_interface {
    public:
        using iterator_type = typename Source::const_iterator;

        using parse_match_type = parse_match<iterator_type, MatchId, TextPosition>;
        using parse_match_container_type = std::vector<parse_match_type>;

        using parse_error_type = parse_error<iterator_type, ErrorId, TextPosition>;
        using parse_error_container_type = std::vector<parse_error_type>;

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_iterator(begin)
            , m_begin(begin)
            , m_end(end)
        {
        }

        parse_context(Source& src)
            : parse_context(src.begin(), src.end())
        {
        }

        const iterator_type& iterator() const {
            return m_iterator;
        }

        const TextPosition& text_position() const {
            return m_text_position;
        }

        const parse_match_container_type& matches() const {
            return m_matches;
        }

        const parse_error_container_type& errors() const {
            return m_errors;
        }

        bool is_parse_valid() const override {
            return m_iterator != m_end;
        }

        bool is_parse_end() const override {
            return m_iterator == m_end;
        }

        bool parse_symbol(int symbol) override {
            if (get_block_terminals_state()) {
                return false;
            }
            if (is_parse_valid()) {
                if (compare(*m_iterator, symbol) == 0) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        bool parse_string(const std::vector<int>& string) override {
            if (get_block_terminals_state()) {
                return false;
            }
            auto itStr = string.begin();
            auto itSrc = m_iterator;
            auto tp = m_text_position;
            for (;;) {
                if (itStr == string.end()) {
                    break;
                }
                if (itSrc == m_end || compare(*itSrc, *itStr) != 0) {
                    return false;
                }
                ++itStr;
                ++itSrc;
                tp.increment_column();
            }
            m_iterator = itSrc;
            m_text_position = tp;
            return true;
        }

        bool parse_set(const std::vector<int>& set) override {
            if (get_block_terminals_state()) {
                return false;
            }
            if (is_parse_valid()) {
                const auto& token = *m_iterator;
                for (const auto& symbol : set) {
                    if (compare(token, symbol) == 0) {
                        increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

        bool parse_range(int min, int max) override {
            if (get_block_terminals_state()) {
                return false;
            }
            if (is_parse_valid()) {
                const auto& token = *m_iterator;
                if (compare(token, min) >= 0 && compare(token, max) <= 0) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        bool parse_newline(int symbol) override {
            if (parse_symbol(symbol)) {
                m_text_position.increment_line();
                return true;
            }
            return false;
        }

        bool parse_newline(const std::vector<int>& string) override {
            if (parse_string(string)) {
                m_text_position.increment_line();
                return true;
            }
            return false;
        }

        bool parse_any() override {
            if (get_block_terminals_state()) {
                return false;
            }
            if (is_parse_valid()) {
                increment_parse_position();
                return true;
            }
            return false;
        }

        bool skip_before(const parse_function_type& fn) override {
            while (m_iterator != m_end) {
                const state prev_state = get_state();
                try {
                    const bool ok = fn(*this);
                    set_state(prev_state);
                    if (ok) {
                        return true;
                    }
                }
                catch (...) {
                    set_state(prev_state);
                    throw;
                }
                increment_parse_position();
            }
            return false;
        }

        bool skip_after(const parse_function_type& fn) override {
            while (m_iterator != m_end) {
                const state prev_state = get_state();
                try {
                    if (fn(*this)) {
                        return true;
                    }
                }
                catch (...) {
                    set_state(prev_state);
                    throw;
                }
                set_state(prev_state);
                increment_parse_position();
            }
            return false;
        }

        bool parse(const parse_function_type& fn) override {
            const state prev_state = get_state();
            try {
                if (fn(*this)) {
                    return true;
                }
            }
            catch (...) {
                set_state(prev_state);
                throw;
            }
            set_state(prev_state);
            return false;
        }

        bool test_parse(const parse_function_type& fn) override {
            const state prev_state = get_state();
            try {
                const bool ok = fn(*this);
                set_state(prev_state);
                return ok;
            }
            catch (...) {
                set_state(prev_state);
                throw;
            }
            return false;
        }

        void push_match_start_state() override {
            m_match_start_state_stack.push_back(get_state());
        }

        void pop_match_start_state() override {
            assert(!m_match_start_state_stack.empty());
            m_match_start_state_stack.pop_back();
        }

        void add_match(int match_id) override {
            assert(!m_match_start_state_stack.empty());
            add_match_from(match_id, m_match_start_state_stack.back());
        }

        virtual bool parse_match_(int match_id, const parse_function_type& fn) {
            assert(get_block_terminals_state() ? !m_left_recursion_match_start_state_stack.empty() : !m_match_start_state_stack.empty());
            const state match_start_state = get_block_terminals_state() ? m_left_recursion_match_start_state_stack.back() : m_match_start_state_stack.back();
            if (fn(*this)) {
                add_match_from(match_id, match_start_state);
                return true;
            }
            return false;
        }

        void push_error_start_state() override {
            m_error_start_state_stack.push_back(get_state());
        }

        void pop_error_start_state() override {
            assert(!m_error_start_state_stack.empty());
            m_error_start_state_stack.pop_back();
        }

        void add_error(int error_id) override {
            assert(!m_error_start_state_stack.empty());
            const state error_start_state = m_error_start_state_stack.back();
            m_errors.push_back(parse_error_type(error_start_state.iterator, m_iterator, static_cast<ErrorId>(error_id), error_start_state.text_position, m_text_position));
        }

        bool parse_loop(const parse_function_type& fn) override {
            for(;;) {
                const state prev_state = get_state();
                try {
                    if (!fn(*this)) {
                        set_state(prev_state);
                        break;
                    }
                }
                catch (...) {
                    set_state(prev_state);
                    throw;
                }
                if (m_iterator == prev_state.iterator) {
                    break;
                }
            }
            return true;
        }

        bool parse_left_recursion(rule* r, const parse_function_type& fn) override {
            const rule_state rs = get_rule_state(r);

            if (m_iterator != rs.iterator) {
                return parse_rule(r, fn);
            }

            switch (rs.status) {
                case rule_status::none:
                    throw left_recursion_exception(r);

                case rule_status::reject:
                    return false;

                case rule_status::accept:
                    set_block_terminals_state(false);
                    return true;
            }

            throw std::runtime_error("parse_context::parse_left_recursion: invalid rule status.");
        }

    private:
        struct state {
            iterator_type iterator;
            TextPosition text_position;
            size_t match_count;
        };

        enum class rule_status {
            none,
            reject,
            accept
        };

        struct rule_state {
            iterator_type iterator;
            rule_status status;
        };

        using rule_state_stack = std::vector<rule_state>;

        iterator_type m_iterator;
        const iterator_type m_begin;
        const iterator_type m_end;
        TextPosition m_text_position;
        const SymbolComparator m_symbol_comparator{};
        std::vector<state> m_state_stack;
        std::vector<state> m_match_start_state_stack;
        std::vector<state> m_error_start_state_stack;
        parse_match_container_type m_matches;
        parse_error_container_type m_errors;
        std::map<rule*, rule_state_stack> m_rule_state_stacks;
        std::vector<bool> m_block_terminals_stack{false};
        std::vector<state> m_left_recursion_match_start_state_stack;

        template <class A, class B>
        int compare(const A& a, const B& b) const {
            return m_symbol_comparator(static_cast<int>(a), static_cast<int>(b));
        }

        void increment_parse_position() {
            ++m_iterator;
            m_text_position.increment_column();
        }

        state get_state() const {
            return { m_iterator, m_text_position, m_matches.size() };
        }

        void set_state(const state& s) {
            m_iterator = s.iterator;
            m_text_position = s.text_position;
            m_matches.resize(s.match_count);
        }

        void add_match_from(int match_id, const state& match_start_state) {
            parse_match_container_type children(m_matches.begin() + match_start_state.match_count, m_matches.end());
            m_matches.resize(match_start_state.match_count);
            m_matches.push_back(parse_match_type(match_start_state.iterator, m_iterator, static_cast<MatchId>(match_id), match_start_state.text_position, m_text_position, std::move(children)));
        }

        rule_state get_rule_state(rule* r) {
            rule_state_stack& rss = m_rule_state_stacks[r];
            if (rss.empty()) {
                rss.push_back(rule_state{ m_begin, rule_status::none });
            }
            return rss.back();
        }

        void push_rule_state(rule* r, const rule_state& state) {
            m_rule_state_stacks[r].push_back(state);
        }

        void pop_rule_state(rule* r) {
            m_rule_state_stacks[r].pop_back();
        }

        void push_left_recursion_match_start_state() {
            m_left_recursion_match_start_state_stack.push_back(get_state());
        }

        void pop_left_recursion_match_start_state() {
            m_left_recursion_match_start_state_stack.pop_back();
        }

        void push_block_terminals_state(bool v) {
            m_block_terminals_stack.push_back(v);
        }

        void pop_block_terminals_state() {
            m_block_terminals_stack.pop_back();
        }

        bool get_block_terminals_state() {
            return m_block_terminals_stack.back();
        }

        void set_block_terminals_state(bool v) {
            m_block_terminals_stack.back() = v;
        }

        bool invoke_rule_parse_function(rule* r, const parse_function_type& fn, rule_status status) {
            push_rule_state(r, rule_state{ m_iterator, status });
            try {
                const bool ok = fn(*this);
                pop_rule_state(r);
                return ok;
            }
            catch (...) {
                pop_rule_state(r);
                throw;
            }
        }

        bool invoke_rule_parse_function_accept(rule* r, const parse_function_type& fn) {
            push_block_terminals_state(true);
            try {
                const bool ok = invoke_rule_parse_function(r, fn, rule_status::accept);
                pop_block_terminals_state();
                return ok;
            }
            catch (...) {
                pop_block_terminals_state();
                throw;
            }
        }

        bool handle_left_recursion(rule* r, const parse_function_type& fn) {
            push_left_recursion_match_start_state();
            try {
                //reject phase
                if (!invoke_rule_parse_function(r, fn, rule_status::reject)) {
                    pop_left_recursion_match_start_state();
                    return false;
                }

                //accept phase
                while (invoke_rule_parse_function_accept(r, fn)) {
                }

                pop_left_recursion_match_start_state();
                return true;
            }
            catch (...) {
                pop_left_recursion_match_start_state();
                throw;
            }
        }

        bool parse_rule(rule* r, const parse_function_type& fn) {
            try {
                return invoke_rule_parse_function(r, fn, rule_status::none );
            }
            catch (left_recursion_exception ex) {
                if (ex.rule() == r) {
                    return handle_left_recursion(r, fn);
                }
                throw ex;
            }
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
