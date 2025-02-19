#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include "match.hpp"
#include "error.hpp"
#include "string_util.hpp"


namespace parserlib {


    template <class ParseDefinitions = parse_definitions<>>
    class parse_context;


    template <class ParseContext> class rule;


    template <class ParseDefinitions> 
    class parse_context : public ParseDefinitions {
    public:
        // types --------------------------------------------------------------

        using input_type = typename ParseDefinitions::input_type;
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;
        using input_token_comparator_type = typename ParseDefinitions::input_token_comparator_type;
        using output_token_type = typename ParseDefinitions::output_token_type;
        using error_id_type = typename ParseDefinitions::error_id_type;
        using input_span_type = typename ParseDefinitions::input_span_type;

        using span_type = span<input_iterator_type>;

        using match_type = match<ParseDefinitions>;
        using match_container_type = std::vector<match_type>;

        using error_type = error<ParseDefinitions>;
        using error_container_type = std::vector<error_type>;

        class state {
        public:
            const input_iterator_type& parse_position() const noexcept {
                return m_parse_position;
            }

            const std::size_t matches_size() const noexcept {
                return m_matches_size;
            }

        private:
            state(const input_iterator_type& parse_position, std::size_t matches_size) noexcept
                : m_parse_position(parse_position)
                , m_matches_size(matches_size)
            {
            }

            input_iterator_type m_parse_position;
            std::size_t m_matches_size;

            friend class parse_context<ParseDefinitions>;
        };

        using state_container_type = std::vector<state>;

        using rule_type = rule<parse_context<ParseDefinitions>>;

        // constructors / destructor ------------------------------------------

        parse_context(input_type& source) noexcept
            : m_source(source)
            , m_parse_position(source.begin())
            , m_first_unparsed_position(source.begin())
        {
        }

        // source and parse positions -----------------------------------------

        const input_type& source() const {
            return m_source;
        }

        input_type& source() {
            return m_source;
        }

        const input_iterator_type& parse_position() const noexcept {
            return m_parse_position;
        }

        input_iterator_type end_position() const noexcept {
            return m_source.end();
        }

        bool is_valid_parse_position() const noexcept {
            return m_parse_position != m_source.end();
        }

        bool is_end_parse_position() const noexcept {
            return m_parse_position == m_source.end();
        }

        void increment_parse_position() noexcept {
            assert(m_parse_position < m_source.end());
            ++m_parse_position;
            update_first_unparsed_position();
        }

        void increment_parse_position(std::size_t count) noexcept {
            assert(m_parse_position + count <= m_source.end());
            m_parse_position += count;
            update_first_unparsed_position();
        }

        const input_iterator_type& first_unparsed_position() const noexcept {
            return m_first_unparsed_position;
        }

        void set_parse_position(const input_iterator_type& pos) noexcept {
            m_parse_position = pos;
            update_first_unparsed_position();
        }

        // token comparison ---------------------------------------------------

        template <class L, class R>
        static int compare_tokens(const L& l, const R& r) noexcept {
            return input_token_comparator_type()(l, r);
        }

        template <class T>
        int compare_tokens_at_parse_position(const T& token) const noexcept {
            return input_token_comparator_type()(*parse_position(), token);
        }

        template <class It>
        int compare_tokens_at_parse_position(const It& begin, const It& end) const noexcept {
            auto pos = m_parse_position;
            auto it = begin;
            for (;;) {
                if (it == end) {
                    break;
                }
                if (pos == m_source.end()) {
                    return -1;
                }
                const int comp_result = compare_tokens(*pos, *it);
                if (comp_result != 0) {
                    return comp_result;
                }
                ++pos;
                ++it;
            }
            return 0;
        }

        template <class T, std::enable_if_t<std::is_member_function_pointer_v<decltype(& T::begin)>, bool> = true>
        int compare_tokens_at_parse_position(const T& sequence) const noexcept {
            return compare_tokens_at_parse_position(sequence.begin(), sequence.end());
        }

        template <class T>
        int compare_tokens_at_parse_position(const T* str) const noexcept {
            return compare_tokens_at_parse_position(str, str + null_terminated_string_length(str));
        }

        template <class T>
        std::size_t get_sequence_length(const T& token) const noexcept {
            return 1;
        }

        template <class T>
        std::size_t get_sequence_length(const T* str) const noexcept {
            return null_terminated_string_length(str);
        }

        template <class T, std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::size)>, bool> = true>
        std::size_t get_sequence_length(const T& container) const noexcept {
            return container.size();
        }

        // matches ------------------------------------------------------------

        const match_container_type& matches() const noexcept {
            return m_matches;
        }

        void add_match(const output_token_type& token, const state& match_start_state, const state& match_end_state) noexcept {
            assert(match_start_state.parse_position() <= match_end_state.parse_position());
            const auto children_begin = m_matches.begin() + match_start_state.matches_size();
            const auto children_end = m_matches.begin() + match_end_state.matches_size();
            std::vector<match_type> children(children_begin, children_end);
            m_matches.erase(children_begin, children_end);
            m_matches.push_back(match_type(token, span_type(match_start_state.parse_position(), match_end_state.parse_position()), std::move(children)));
        }

        // state --------------------------------------------------------------

        state get_state() const noexcept {
            return { m_parse_position, m_matches.size() };
        }

        void set_state(const state& s) noexcept {
            m_parse_position = s.parse_position();
            m_matches.resize(s.matches_size());
        }

        // error handling -----------------------------------------------------

        const error_container_type& errors() const noexcept {
            return m_errors;
        }

        const error_type& error() const noexcept {
            return m_error;
        }

        void set_error(const error_type& error) noexcept {
            m_error = error;
        }

        void set_error(const error_id_type& error_id, const input_iterator_type& begin, const input_iterator_type& end, const input_iterator_type& error_pos) noexcept {
            set_error(error_type(error_id, input_span_type(begin, end), error_pos));
        }

        void commit_error() noexcept {
            m_errors.push_back(m_error);
            m_error = error_type();
        }

        void add_error(const error_id_type& error_id, const input_iterator_type& begin, const input_iterator_type& end, const input_iterator_type& error_pos) noexcept {
            set_error(error_id, begin, end, error_pos);
            commit_error();
        }

        // left recursion -----------------------------------------------------

        bool is_rule_left_recursive(const rule_type& rule) const noexcept {
            auto it = m_rule_parse_positions.find(rule.pointer_to_derived());
            return it != m_rule_parse_positions.end() && !it->second.empty() && it->second.back() == m_parse_position;
        }

        void push_rule_parse_position(const rule_type& rule) {
            m_rule_parse_positions[rule.pointer_to_derived()].push_back(m_parse_position);
        }

        void pop_rule_parse_position(const rule_type& rule) {
            auto it = m_rule_parse_positions.find(rule.pointer_to_derived());
            assert(it != m_rule_parse_positions.end());
            assert(it->second.size() > 0);
            it->second.pop_back();
        }

    private:
        //source type, parse positions
        input_type& m_source;
        input_iterator_type m_parse_position;
        input_iterator_type m_first_unparsed_position;

        //matches
        match_container_type m_matches;

        //error handling
        error_container_type m_errors;
        error_type m_error;

        //left recursion
        std::map<const rule_type*, std::vector<input_iterator_type>> m_rule_parse_positions;

        void update_first_unparsed_position() noexcept {
            if (m_parse_position > m_first_unparsed_position) {
                m_first_unparsed_position = m_parse_position;
            }
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
