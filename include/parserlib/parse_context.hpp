#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include "match.hpp"
#include "parse_error.hpp"
#include "default_symbol_comparator.hpp"
#include "parse_iterator.hpp"


namespace parserlib {


    template <
        class Iterator = typename std::string::const_iterator, 
        class MatchId = int, 
        class ErrorId = int, 
        class SymbolComparator = default_symbol_comparator
    >
    class parse_context {
    public:
        using iterator_type = Iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using symbol_comparator_type = default_symbol_comparator;

        using match_type = match<match_id_type, iterator_type>;
        using match_container_type = std::vector<match_type>;

        using parse_error_type = parse_error<error_id_type, iterator_type>;
        using parse_error_container_type = std::vector<parse_error_type>;

        using parse_context_type = parse_context<Iterator, MatchId, ErrorId, SymbolComparator>;

        class state {
        public:
            const iterator_type& begin() const noexcept {
                return m_iterator;
            }

            const iterator_type& end() const noexcept {
                return m_iterator;
            }

            size_t get_match_count() const noexcept {
                return m_match_count;
            }

        private:
            iterator_type m_begin;
            iterator_type m_end;
            size_t m_match_count;

            state(const iterator_type& begin, const iterator_type& end, size_t mc) 
                : m_begin(it)
                , m_end(end)
                , m_match_count(mc)
            {
            }

            friend parse_context_type;
        };

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state(begin, end, 0)
            , m_end(end)
        {
        }

        bool is_valid_parse_position() const {
            return m_state.m_begin != m_state.m_end;
        }

        auto get_current_symbol() const {
            return *m_state.m_begin;
        }

        template <class L, class R>
        static int compare_symbols(const L& left, const R& right) noexcept {
            return symbol_comparator_type::compare(left, right);
        }

        void increment_parse_position() {
            ++m_state.m_begin;
        }

        void increment_parse_position(size_t count) {
            m_state.m_begin += count;
        }

        void increment_text_position_line() {
            _increment_text_position_line(m_state.m_begin);
        }

        const state& get_state() const noexcept {
            return m_state;
        }

        void set_state(const state& s) {
            m_state = state;
            m_matches.resize(s.m_match_count);
        }

        const match_container_type& get_matches() const noexcept {
            return m_matches;
        }

        void add_match(const match_id_type& id, const state& begin_state, const state& end_state) {
            match_container_type children{ m_matches.begin() + begin_state.m_match_count, m_matches.end() };
            m_matches.resize(begin_state.m_match_count);
            m_matches.emplace_back(id, begin_state.m_begin, end_state.m_begin, std::move(children));
        }

        const error_container_type& get_errors() const noexcept {
            return m_errors;
        }

        void add_error(const error_id_type& id, const state& begin_state, const state& end_state) {
            m_errors.emplace_back(id, begin_state.m_begin, end_state.m_begin);
        }

    private:
        state m_state;
        const iterator_type m_end;
        match_container_type m_matches;
        error_container_type m_errors;

        template <class T> 
        static void _increment_text_position_line(T& it) {
        }

        template <class It, class Tp>
        static void _increment_text_position_line(parse_iterator<It, Tp>& pit) {
            pit.increment_text_position_line();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
