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
#include "is_container.hpp"


namespace parserlib {


    template <class ParseDefinitions = parse_definitions<>>
    class parse_context;


    template <class ParseContext> class rule;


    /**
     * Holds the data for parsing.
     * An instance of it is passed down the grammar tree,
     * each object using and manipulating the context as required.
     * @param ParseDefinitions parse definitions.
     */
    template <class ParseDefinitions> 
    class parse_context : public ParseDefinitions {
    public:
        // types --------------------------------------------------------------

        /** Input type. */
        using input_type = typename ParseDefinitions::input_type;

        /** Input iterator type. */
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;

        /** Input token comparator type. */
        using input_token_comparator_type = typename ParseDefinitions::input_token_comparator_type;

        /** Input span type. */
        using input_span_type = typename ParseDefinitions::input_span_type;

        /** Input token type. */
        using input_token_type = typename ParseDefinitions::input_token_type;

        /** Output token type. */
        using output_token_type = typename ParseDefinitions::output_token_type;

        /** Match type. */
        using match_type = match<ParseDefinitions>;

        /** Match container type. */
        using match_container_type = std::vector<match_type>;

        /** Error id type. */
        using error_id_type = typename ParseDefinitions::error_id_type;

        /** Error type. */
        using error_type = error<ParseDefinitions>;

        /** Error container type. */
        using error_container_type = std::vector<error_type>;

        /**
         * Context state. 
         */
        class state {
        public:
            /**
             * Returns the parse position stored in this state.
             * @return the parse position stored in this state.
             */
            const input_iterator_type& parse_position() const noexcept {
                return m_parse_position;
            }

            /**
             * Returns the size of the matches internal stack.
             * @return the size of the matches internal stack.
             */
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

        /** State container type. */
        using state_container_type = std::vector<state>;

        /** Rule type */
        using rule_type = rule<parse_context<ParseDefinitions>>;

        // constructors / destructor ------------------------------------------

        /**
         * The constructor.
         * @param input the input (the source).
         */
        parse_context(input_type& input) noexcept
            : m_input(input)
            , m_parse_position(m_input.begin())
            , m_first_unparsed_position(m_input.begin())
        {
        }

        // input and iterators ------------------------------------------------

        /**
         * Returns the input.
         * @return the input.
         */
        const input_type& input() const {
            return m_input;
        }

        /**
         * Returns the current parse position.
         * @return the current parse position.
         */
        const input_iterator_type& parse_position() const noexcept {
            return m_parse_position;
        }

        /**
         * Returns the end position.
         * @return the end position.
         */
        input_iterator_type end_position() const noexcept {
            return m_input.end();
        }

        /**
         * Checks if the current parse position has reached the input end.
         * @return true if the current parse position does not equal the end position, false otherwise.
         */
        bool is_valid_parse_position() const noexcept {
            return m_parse_position != m_input.end();
        }

        /**
         * Checks if the current parse position has reached the input end.
         * @return true if the current parse position equals the end position, false otherwise.
         */
        bool is_end_parse_position() const noexcept {
            return m_parse_position == m_input.end();
        }

        /**
         * Increments the parse position by 1. 
         */
        void increment_parse_position() noexcept {
            ++m_parse_position;
            update_first_unparsed_position();
        }

        /**
         * Increments the parse position by by the given count.
         * @param count number of positions to increment the parse position.
         */
        void increment_parse_position(std::size_t count) noexcept {
            m_parse_position += count;
            update_first_unparsed_position();
        }

        /**
         * Returns the first position that has never been parsed yet.
         * Used in managing errors (the longest parsing is the most valid).
         * @return the first position that is not parsed yet.
         */
        const input_iterator_type& first_unparsed_position() const noexcept {
            return m_first_unparsed_position;
        }

        /**
         * Sets the parse position.
         * @param pos the new parse position.
         */
        void set_parse_position(const input_iterator_type& pos) noexcept {
            m_parse_position = pos;
            update_first_unparsed_position();
        }

        /**
         * Invokes the function `increment_line()` on the current parse position.
         * The input iterator type must provide this function.
         */
        void increment_line() {
            m_parse_position.increment_line();
        }

        // token comparison ---------------------------------------------------

        /**
         * Invokes the input token comparator defined in ParseDefinitions.
         * @param l left operand.
         * @param r right operand.
         * @return the result of the input token comparator.
         */
        template <class L, class R>
        static int compare_tokens(const L& l, const R& r) noexcept {
            return input_token_comparator_type()(l, r);
        }

        /**
         * Invokes the input token comparator defined in ParseDefinitions,
         * for the given token and the token at the current parse position.
         * @param token token to compare, passed as right operand to the comparator.
         * @return the result of the input token comparator.
         */
        template <class T>
        int compare_tokens_at_parse_position(const T& token) const noexcept {
            return input_token_comparator_type()(*parse_position(), token);
        }

        /**
         * Invokes the input token comparator defined in ParseDefinitions,
         * for the given sequence of tokens and the tokens at the current parse position.
         * @param begin iterator to the start of the sequence.
         * @param end iterator to the end of the sequence.
         * @return the result of the input token comparator for the first pair of tokens that are not equal.
         */
        template <class It>
        int compare_tokens_at_parse_position(const It& begin, const It& end) const noexcept {
            auto pos = m_parse_position;
            auto it = begin;
            for (;;) {
                if (it == end) {
                    break;
                }
                if (pos == m_input.end()) {
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

        /**
         * Invokes the input token comparator defined in ParseDefinitions,
         * for the given container of tokens and the tokens at the current parse position.
         * Valid only if T has `cbegin()` and `cend()` functions.
         * @param container the container with the tokens to test against the input.
         * @return the result of the input token comparator for the first pair of tokens that are not equal.
         */
        template <class T, std::enable_if_t<is_container_v<T>, bool> = true>
        int compare_tokens_at_parse_position(const T& container) const noexcept {
            return compare_tokens_at_parse_position(container.begin(), container.end());
        }

        /**
         * Invokes the input token comparator defined in ParseDefinitions,
         * for the given null-terminated string of tokens and the tokens at the current parse position.
         * Valid only if T has `cbegin()` and `cend()` functions.
         * @param str the null-terminated string to compare to the input.
         * @return the result of the input token comparator for the first pair of tokens that are not equal.
         */
        template <class T>
        int compare_tokens_at_parse_position(const T* str) const noexcept {
            return compare_tokens_at_parse_position(str, str + null_terminated_string_length(str));
        }

        /**
         * Returns the sequence length for one token.
         * @param token token.
         * @return 1.
         */
        template <class T>
        std::size_t get_sequence_length(const T& token) const noexcept {
            return 1;
        }

        /**
         * Returns the sequence length for a container.
         * @param container the container to get the length of.
         * @return the length of the container (`container.size()`).
         */
        template <class T, std::enable_if_t<is_container_v<T>, bool> = true>
        std::size_t get_sequence_length(const T& container) const noexcept {
            return container.size();
        }

        /**
         * Returns the sequence length for a null-terminated string.
         * @param str the null-terminated string to get the length of.
         * @return the length of the null-terminated string.
         */
        template <class T>
        std::size_t get_sequence_length(const T* str) const noexcept {
            return null_terminated_string_length(str);
        }

        // matches ------------------------------------------------------------

        /**
         * Returns the matches identified currently for this context.
         * @return the matches.
         */
        const match_container_type& matches() const noexcept {
            return m_matches;
        }

        /**
         * Adds a match to the context.
         * @param id the id of the match.
         * @param match_start_state state the signifies the start of the match.
         * @param match_end_state state the signifies the end of the match.
         */
        void add_match(const output_token_type& id, const state& match_start_state, const state& match_end_state) noexcept {
            assert(match_start_state.parse_position() <= match_end_state.parse_position());
            const auto children_begin = m_matches.begin() + match_start_state.matches_size();
            const auto children_end = m_matches.begin() + match_end_state.matches_size();
            std::vector<match_type> children(children_begin, children_end);
            m_matches.erase(children_begin, children_end);
            m_matches.push_back(match_type(id, input_span_type(match_start_state.parse_position(), match_end_state.parse_position()), std::move(children)));
        }

        // state --------------------------------------------------------------

        /**
         * Returns the current state of the context.
         * @return the current state of the context.
         */
        state get_state() const noexcept {
            return { m_parse_position, m_matches.size() };
        }

        /**
         * Sets the state of the context.
         * @param s the state to set.
         */
        void set_state(const state& s) noexcept {
            m_parse_position = s.parse_position();
            m_matches.resize(s.matches_size());
        }

        // error handling -----------------------------------------------------
        
        /**
         * Returns the errors identified currently for this context.
         * @return the errors.
         */
        const error_container_type& errors() const noexcept {
            return m_errors;
        }

        /**
         * Returns the current error.
         * @return the current error.
         */
        const error_type& error() const noexcept {
            return m_error;
        }

        /**
         * Sets the current error, only if no current error exists in the context
         * or if the new error starts at a greater position than the current one
         * within the source.
         * @param error the current error.
         * @return true if the error was set, false if there is an error in a greater position.
         */
        bool set_error(const error_type& error) noexcept {
            if (m_error.invalid() || error.position() > m_error.position()) {
                m_error = error;
                return true;
            }
            return false;
        }

        /**
         * Sets the current error.
         * @param error_id the error id.
         * @param span the area of the source that the error is within.
         * @param error_pos position that the error starts.
         * @return true if the error was set, false if there is an error in a greater position.
         */
        bool set_error(const error_id_type& error_id, const input_span_type& span, const input_iterator_type& error_pos) noexcept {
            return set_error(error_type(error_id, span, error_pos));
        }

        /**
         * Sets the current error.
         * @param error_id the error id.
         * @param begin start position of span that contains the error.
         * @param end end position of the span that contains the error.
         * @param error_pos position that the error starts.
         */
        bool set_error(const error_id_type& error_id, const input_iterator_type& begin, const input_iterator_type& end, const input_iterator_type& error_pos) noexcept {
            return set_error(error_id, input_span_type(begin, end), error_pos);
        }

        /**
         * If the current error is valid, then it is added to the internal container of errors,
         * and the current error is reset to an invalid one.
         */
        void commit_error() noexcept {
            if (m_error.valid()) {
                m_errors.push_back(m_error);
                m_error = error_type();
            }
        }

        /**
         * Adds an error.
         * It sets the current error, then commits it.
         * @param error_id the error id.
         * @param span the area of the source that the error is within.
         * @param error_pos position that the error starts.
         * @return true if the error was set, false if there is an error in a greater position.
         */
        bool add_error(const error_id_type& error_id, const input_span_type& span, const input_iterator_type& error_pos) noexcept {
            if (set_error(error_id, span, error_pos)) {
                commit_error();
                return true;
            }
            return false;
        }

        /**
         * Adds an error.
         * It sets the current error, then commits it.
         * @param error_id the error id.
         * @param begin start position of span that contains the error.
         * @param end end position of the span that contains the error.
         * @param error_pos position that the error starts.
         * @return true if the error was set, false if there is an error in a greater position.
         */
        bool add_error(const error_id_type& error_id, const input_iterator_type& begin, const input_iterator_type& end, const input_iterator_type& error_pos) noexcept {
            return add_error(error_id, input_span_type(begin, end), error_pos);
        }

        // left recursion -----------------------------------------------------

        /**
         * Checks if the given rule is left-recursive at the current parse position.
         * @param rule rule to check for.
         * @return true if the given rule is left-recursive at the current parse position, false otherwise.
         */
        bool is_rule_left_recursive(const rule_type& rule) const noexcept {
            auto it = m_rule_parse_positions.find(rule.pointer_to_derived());
            return it != m_rule_parse_positions.end() && !it->second.empty() && it->second.back() == m_parse_position;
        }

        /**
         * Pushes the current parse position as the parse position for the given rule.
         * @param rule rule to push the current parse position as the rule's parse position.
         */
        void push_rule_parse_position(const rule_type& rule) {
            m_rule_parse_positions[rule.pointer_to_derived()].push_back(m_parse_position);
        }

        /**
         * Pops the top of the internal parse position stack for the given rule.
         * @param rule the rule to pop the parse position for.
         */
        void pop_rule_parse_position(const rule_type& rule) {
            auto it = m_rule_parse_positions.find(rule.pointer_to_derived());
            assert(it != m_rule_parse_positions.end());
            assert(it->second.size() > 0);
            it->second.pop_back();
        }

    private:
        //input type and iterators
        input_type& m_input;
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
