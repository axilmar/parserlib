#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <string>
#include <cctype>
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
            if (is_parse_valid()) {
                if (compare(*m_iterator, symbol) == 0) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        bool parse_string(const std::vector<int>& string) override {
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
            const state& match_start_state = m_match_start_state_stack.back();
            m_matches.push_back(parse_match_type(match_start_state.iterator, m_iterator, static_cast<MatchId>(match_id), match_start_state.text_position, m_text_position));
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

        bool parse_loop(const parse_function_type& fn) {
            while (is_parse_valid()) {
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

    private:
        struct state {
            iterator_type iterator;
            TextPosition text_position;
            size_t match_count;
        };

        iterator_type m_iterator;
        const iterator_type m_end;
        TextPosition m_text_position;
        std::vector<state> m_state_stack;
        std::vector<state> m_match_start_state_stack;
        std::vector<state> m_error_start_state_stack;
        parse_match_container_type m_matches;
        parse_error_container_type m_errors;
        const SymbolComparator m_symbol_comparator;

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
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
