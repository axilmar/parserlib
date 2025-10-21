#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <string>
#include <cassert>
#include <algorithm>
#include <vector>
#include "parse_context_traits.hpp"


namespace parserlib {


    template <class String = std::string, class Traits = parse_context_traits<String>> class parse_context {
    public:
        using traits_type = Traits;

        using string_type = typename Traits::string_type;

        using value_type = typename Traits::value_type;

        using iterator_type = typename Traits::iterator_type;

        using position_type = typename Traits::position;

        using match_id_type = typename Traits::match_id_type;

        class parse_position {
        public:
            parse_position() {
            }

            const iterator_type& iterator() const {
                return m_iterator;
            }

            const position_type& position() const {
                return m_position;
            }

        private:
            iterator_type m_iterator;
            position_type m_position;

            parse_position(const iterator_type& it, const position_type& pos) : m_iterator(it), m_position(pos) {
            }

            friend class parse_context<String, Traits>;
        };

        class match;

        using match_container_type = std::vector<match>;

        class match {
        public:
            match() {
            }

            const parse_position& start_position() const {
                return m_start_position;
            }

            const parse_position& end_position() const {
                return m_end_position;
            }

            const match_id_type& match_id() const {
                return m_match_id;
            }

        private:
            parse_position m_start_position;
            parse_position m_end_position;
            match_id_type m_match_id;
            match_container_type m_children;

            match(const parse_position& start, const parse_position& end, const match_id_type& id, match_container_type&& children)
                : m_start_position(start), m_end_position(end), m_match_id(id), m_children(std::move(children))
            {
            }
        };

        parse_context(String& string)
            : m_string(string)
            , m_parse_position(string.begin(), position_type())
        {
        }

        bool valid() const {
            return m_parse_position.m_iterator != m_string.end();
        }

        template <class T> bool parse_symbol(const T& value) {
            if (valid() && Traits::to_lower(*m_parse_position.m_iterator) == Traits::to_lower(value)) {
                increment_parse_position();
                return true;
            }
            return false;
        }

        template <class T> bool parse_symbol_string(const T& string) {
            iterator_type this_it = m_parse_position.m_iterator;
            auto string_it = string.begin();
            for(;;) {
                if (string_it == string.end()) {
                    break;
                }
                if (this_it == m_string.end()) {
                    return false;
                }
                if (Traits::to_lower(*this_it) != Traits::to_lower(*string_it)) {
                    return false;
                }
                ++this_it;
                ++string_it;
            }
            const size_t count = this_it - m_parse_position.m_iterator;
            m_parse_position.m_position.increment_column(count);
            m_parse_position.m_iterator = this_it;
            return true;
        }

        template <class T> bool parse_symbol_set(const T& string) {
            assert(std::is_sorted(string.begin(), string.end()));
            if (valid()) {
                const auto symbol = Traits::to_lower(*m_parse_position.m_iterator);
                auto it = std::upper_bound(string.begin(), string.end(), symbol, [](const auto& a, const auto& b) {
                    return a - Traits::to_lower(b);
                });
                if (it != string.begin() && symbol == Traits::to_lower(*std::prev(it))) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        template <class T> bool parse_symbol_range(const T& min, const T& max) {
            assert(min <= max);
            if (valid()) {
                const auto symbol = Traits::to_lower(*m_parse_position.m_iterator);
                if (symbol >= Traits::to_lower(min) && symbol <= Traits::to_lower(max)) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        template <class F>
        bool parse_and_restore_state(const F& func) {
            state s = get_state();
            bool r;
            try {
                r = func(*this);
            }
            catch (...) {
                set_state(s);
                throw;
            }
            set_state(s);
            return r;
        }

        template <class F>
        bool parse_and_restore_state_on_error(const F& func) {
            state s = get_state();
            bool r;
            try {
                r = func(*this);
            }
            catch (...) {
                set_state(s);
                throw;
            }
            if (!r) {
                set_state(s);
            }
            return r;
        }

        template <class Parser> bool parse_match(const Parser& parser, const match_id_type& match_id) {
            const state start_state = get_state();
            if (parser.parse(*this)) {
                add_match(start_state(), get_state(), match_id);
                return true;
            }
            return false;
        }

        const match_container_type& matches() const {
            return m_matches;
        }

    private:
        struct state {
            class parse_position parse_position;
            size_t match_count;
        };

        String& m_string;
        parse_position m_parse_position;
        match_container_type m_matches;

        void increment_parse_position() {
            ++m_parse_position.m_iterator;
            m_parse_position.m_position.increment_column();
        }

        state get_state() const {
            return state{ m_parse_position, m_matches.size() };
        }

        void set_state(const state& s)  {
            m_parse_position = s.parse_position;
            m_matches.resize(s.match_count);
        }

        void add_match(const state& start_state, const state& end_state, const match_id_type& match_id) {
            match_container_type children(std::next(m_matches.begin(), start_state.match_count), std::next(m_matches.begin(), end_state.match_count));
            m_matches.resize(start_state.match_count);
            m_matches.push_back(match(start_state.parse_position, end_state.parse_position, match_id), std::move(children));
        }
    };


} //namespace parserlib


#endif // PARSERLIB_PARSE_CONTEXT_HPP
