#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <string>
#include <cassert>
#include <algorithm>
#include <vector>
#include <map>
#include <stdexcept>
#include "parse_context_traits.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class String = std::string, class Traits = parse_context_traits<String>> class parse_context {
    public:
        using traits_type = Traits;

        using string_type = typename Traits::string_type;

        using value_type = typename Traits::value_type;

        using iterator_type = typename Traits::iterator_type;

        using text_position_type = typename Traits::text_position_type;

        using match_id_type = typename Traits::match_id_type;

        class parse_position {
        public:
            parse_position() {
            }

            const iterator_type& iterator() const {
                return m_iterator;
            }

            const text_position_type& text_position() const {
                return m_text_position;
            }

        private:
            iterator_type m_iterator;
            text_position_type m_text_position;

            parse_position(const iterator_type& it, const text_position_type& pos) : m_iterator(it), m_text_position(pos) {
            }

            friend class parse_context<String, Traits>;
        };

        class match;

        using match_container_type = std::vector<match>;

        class match {
        public:
            match() {
            }

            const class parse_position& start_position() const {
                return m_start_position;
            }

            const class parse_position& end_position() const {
                return m_end_position;
            }

            const match_id_type& match_id() const {
                return m_match_id;
            }

            const match_container_type& matches() const {
                return m_children;
            }

            String source() const {
                return String(m_start_position.iterator(), m_end_position.iterator());
            }

        private:
            match_id_type m_match_id;
            class parse_position m_start_position;
            class parse_position m_end_position;
            match_container_type m_children;

            match(const match_id_type& id, const class parse_position& start, const class parse_position& end, match_container_type&& children)
                : m_match_id(id), m_start_position(start), m_end_position(end), m_children(std::move(children))
            {
            }

            friend class parse_context<String, Traits>;
        };

        using rule_type = rule<parse_context<String, Traits>>;

        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_parse_position(begin, text_position_type())
            , m_end(end)
        {
        }

        parse_context(const String& string)
            : parse_context(string.begin(), string.end())
        {
        }

        bool valid() const {
            return m_parse_position.m_iterator != m_end;
        }

        template <class T> bool parse_symbol(const T& value) {
            if (m_terminal_parsing_locked) {
                return false;
            }
            if (valid() && Traits::to_lower(*m_parse_position.m_iterator) == Traits::to_lower(value)) {
                increment_parse_position();
                return true;
            }
            return false;
        }

        template <class T> bool parse_symbol_string(const T& string) {
            if (m_terminal_parsing_locked) {
                return false;
            }
            iterator_type this_it = m_parse_position.m_iterator;
            auto string_it = string.begin();
            for(;;) {
                if (string_it == string.end()) {
                    break;
                }
                if (this_it == m_end) {
                    return false;
                }
                if (Traits::to_lower(*this_it) != Traits::to_lower(*string_it)) {
                    return false;
                }
                ++this_it;
                ++string_it;
            }
            const size_t count = this_it - m_parse_position.m_iterator;
            m_parse_position.m_text_position.increment_column(count);
            m_parse_position.m_iterator = this_it;
            return true;
        }

        template <class T> bool parse_symbol_set(const T& string) {
            if (m_terminal_parsing_locked) {
                return false;
            }
            assert(std::is_sorted(string.begin(), string.end()));
            if (valid()) {
                const auto symbol = Traits::to_lower(*m_parse_position.m_iterator);
                auto it = std::upper_bound(string.begin(), string.end(), symbol, [](const auto& a, const auto& b) {
                    return a < Traits::to_lower(b);
                });
                if (it != string.begin()) {
                    const auto set_symbol = Traits::to_lower(*std::prev(it));
                    if (symbol == set_symbol) {
                        increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

        template <class T> bool parse_symbol_range(const T& min, const T& max) {
            if (m_terminal_parsing_locked) {
                return false;
            }
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
            const state start_state = m_terminal_parsing_locked ? m_left_recursion_start_state : get_state();
            if (parser.parse(*this)) {
                add_match(start_state, get_state(), match_id);
                return true;
            }
            return false;
        }

        const match_container_type& matches() const {
            return m_matches;
        }

        bool parse_rule(const rule_type& rule) {
            auto it = m_rule_states.find(rule.this_());

            //first time entering the rule
            if (it == m_rule_states.end()) {
                const auto [it1, ok] = m_rule_states.insert(std::make_pair(rule.this_(), rule_state{ m_parse_position.iterator(), rule_left_recursion_state::none }));
                try {
                    const bool result = rule.parser()->parse(*this);
                    m_rule_states.erase(it1);
                    return result;
                }
                catch (left_recursion_exception ex) {
                    if (ex.rule == rule.this_()) {
                        const bool result = handle_left_recursion(rule, it1->second);
                        m_rule_states.erase(it1);
                        return result;
                    }
                    m_rule_states.erase(it1);
                    throw ex;
                }
                catch (...) {
                    m_rule_states.erase(it1);
                    throw;
                }
            }

            //reentering the rule without left recursion
            else if (m_parse_position.iterator() != it->second.parse_position) {
                rule_state prior_rule_state = it->second;
                it->second.parse_position = m_parse_position.iterator();
                it->second.left_recursion_state = rule_left_recursion_state::none;
                try {
                    const bool result = rule.parser()->parse(*this);
                    it->second = prior_rule_state;
                    return result;
                }
                catch (left_recursion_exception ex) {
                    if (ex.rule == rule.this_()) {
                        const bool result = handle_left_recursion(rule, it->second);
                        it->second = prior_rule_state;
                        return result;
                    }
                    it->second = prior_rule_state;
                    throw ex;
                }
                catch (...) {
                    it->second = prior_rule_state;
                    throw;
                }
            }

            //else left recursion
            else {
                switch (it->second.left_recursion_state) {
                    case rule_left_recursion_state::none:
                        throw left_recursion_exception{rule.this_()};

                    case rule_left_recursion_state::reject:
                        return false;

                    case rule_left_recursion_state::accept:
                        m_terminal_parsing_locked = false;
                        return true;
                }
            }

            throw std::logic_error("parse_context::parse_rule: invalid state");
        }

        const class parse_position& parse_position() const {
            return m_parse_position;
        };

    private:
        struct state {
            class parse_position parse_position;
            size_t match_count;
            bool parsing_locked;
        };

        enum class rule_left_recursion_state {
            none,
            reject,
            accept
        };

        struct left_recursion_exception {
            const rule_type* rule;
        };

        struct rule_state {
            iterator_type parse_position;
            rule_left_recursion_state left_recursion_state;
        };

        class parse_position m_parse_position;
        iterator_type m_end;
        state m_left_recursion_start_state;
        bool m_terminal_parsing_locked{false};
        match_container_type m_matches;
        std::map<const rule_type*, rule_state> m_rule_states;

        void increment_parse_position() {
            Traits::increment_parse_position(m_parse_position.m_iterator, m_end, m_parse_position.m_text_position);
        }

        state get_state() const {
            return state{ m_parse_position, m_matches.size(), m_terminal_parsing_locked };
        }

        void set_state(const state& s)  {
            m_parse_position = s.parse_position;
            m_matches.resize(s.match_count);
            m_terminal_parsing_locked = s.parsing_locked;
        }

        void add_match(const state& start_state, const state& end_state, const match_id_type& match_id) {
            match_container_type children(std::next(m_matches.begin(), start_state.match_count), std::next(m_matches.begin(), end_state.match_count));
            m_matches.resize(start_state.match_count);
            m_matches.push_back(match(match_id, start_state.parse_position, end_state.parse_position, std::move(children)));
        }

        bool handle_left_recursion(const rule_type& rule, struct rule_state& rule_state) {
            const state left_recursion_start_state = get_state();
            m_left_recursion_start_state = left_recursion_start_state;

            rule_state.left_recursion_state = rule_left_recursion_state::reject;
            if (!rule.parser()->parse(*this)) {
                return false;
            }

            rule_state.left_recursion_state = rule_left_recursion_state::accept;
            while (true) {
                m_left_recursion_start_state = left_recursion_start_state;
                rule_state.parse_position = m_parse_position.iterator();
                m_terminal_parsing_locked = true;
                if (!rule.parser()->parse(*this)) {
                    break;
                }
            }

            m_terminal_parsing_locked = false;
            return true;
        }
    };


} //namespace parserlib


#endif // PARSERLIB_PARSE_CONTEXT_HPP
