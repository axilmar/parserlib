#ifndef PARSERLIB_SOURCE_RANGE_PARSE_CONTEXT_HPP
#define PARSERLIB_SOURCE_RANGE_PARSE_CONTEXT_HPP


#include <string>
#include <map>
#include <cassert>
#include "parse_context.hpp"
#include "match.hpp"
#include "parse_error.hpp"
#include "default_source_position.hpp"
#include "default_symbol_comparator.hpp"


namespace parserlib {


    /**
     * A parse context class used for parsing a source range.
     * @param Iterator iterator type.
     * @param MatchId match id type.
     * @param ErrorId error id type.
     * @param SourcePosition source position type.
     * @param SymbolComparator symbol comparator type.
     */
    template <
        class Iterator = typename std::string::const_iterator, 
        class MatchId = int, 
        class ErrorId = int, 
        class SourcePosition = default_source_position, 
        class SymbolComparator = default_symbol_comparator
    >
    class source_range_parse_context : public parse_context {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /** Match id type. */
        using match_id_type = MatchId;

        /** Error id type. */
        using error_id_type = ErrorId;

        /** Source position type. */
        using source_position_type = SourcePosition;

        /** Symbol comparator type. */
        using symbol_comparator_type = SymbolComparator;

        /** Parse position type. */
        using parse_position_type = parse_position<iterator_type, source_position_type>;

        /** Match type. */
        using match_type = match<match_id_type, iterator_type, source_position_type>;

        /** Match container type. */
        using match_container_type = std::vector<match_type>;

        /** Parse error type. */
        using parse_error_type = parse_error<error_id_type, iterator_type, source_position_type>;

        /** Parse error container type. */
        using parse_error_container_type = std::vector<parse_error_type>;

        source_range_parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state{ parse_position_type(begin), 0, end }
            , m_match_start_state{ parse_position_type(begin), 0 }
            , m_error_start_state{ parse_position_type(begin) }
            , m_initial_parse_node_state{ left_recursion_state{ end, left_recursion_state_type::no_left_recursion}, {}}
            , m_end_iterator(end)
        {
        }

        /**
         * Tests whether the end position is reached or not.
         * @return true if the end position is not reached, false otherwise.
         */
        bool is_valid_parse_position() const final {
            return m_state.parse_position.get_iterator() != m_state.end;
        }

        /**
         * Increments the current parse position by one symbol.
         */
        void increment_parse_position() final {
            m_state.parse_position.increment();
            m_match_start_state.parse_position = m_state.parse_position;
            m_error_start_state.parse_position = m_state.parse_position;
        }

        /**
         * Increments the current parse position by a count.
         * @param count number of symbols to increment the parse position.
         */
        void increment_parse_position(std::size_t count) final {
            m_state.parse_position.increment(count);
            m_match_start_state.parse_position = m_state.parse_position;
            m_error_start_state.parse_position = m_state.parse_position;
        }

        /**
         * Retrieves the current symbol, i.e. the symbol at the current parse position.
         * @return the current symbol.
         */
        int get_current_symbol() const final {
            return static_cast<int>(*m_state.parse_position.get_iterator());
        }

        /**
         * Compares two symbols.
         * @param left the left symbol to compare.
         * @param right the right symbol to compare.
         * @return less than 0 if left < right, 0 if left == right, greater than 0 if left > right.
         */
        int compare_symbols(int left, int right) const final {
            return symbol_comparator_type()(left, right);
        }

        /**
         * Parses a single symbol.
         * @param symbol symbol to compare.
         * @return true on success, false on failure.
         */
        bool parse_symbol(int symbol) final {
            if (is_valid_parse_position()) {
                if (compare_symbols(get_current_symbol(), symbol) == 0) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        /**
         * Parses the given symbol sequence.
         * @param sequence the sequence of symbols to compare.
         * @return true on success, false on failure.
         */
        bool parse_symbol_sequence(const std::vector<int>& sequence) final {
            if (is_valid_parse_position()) {
                auto itSequence = sequence.begin();
                auto itSource = m_state.parse_position.get_iterator();
                for (;;) {
                    if (itSequence == sequence.end()) {
                        increment_parse_position(sequence.size());
                        return true;
                    }
                    if (itSource == m_state.end || symbol_comparator_type()(*itSource, *itSequence)) {
                        break;
                    }
                    ++itSequence;
                    ++itSource;
                }
            }
            return false;
        }

        /**
         * Parses the symbol at the current parse position out of a set of symbols.
         * @param set the set of symbols to compare.
         * @return true if the current symbol is within the given set, false otherwise.
         */
        bool parse_symbol_set(const std::vector<int>& set) final {
            if (is_valid_parse_position()) {
                const int current_symbol = get_current_symbol();
                for (const int symbol : set) {
                    const int result = compare_symbols(current_symbol, symbol);
                    if (!result) {
                        increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

        /**
         * Parses the symbol at the current parse position within a symbol range.
         * @param min min symbol value.
         * @param max max symbol value.
         * @return true if the current symbol is within the given range, false otherwise.
         */
        bool parse_symbol_range(int min, int max) final {
            assert(min <= max);
            if (is_valid_parse_position()) {
                const int current_symbol = get_current_symbol();
                if (compare_symbols(current_symbol, min) >= 0 && compare_symbols(current_symbol, max) <= 0) {
                    increment_parse_position();
                    return true;
                }
            }
            return false;
        }

        /**
         * Saves the current parse state into an internal stack.
         */ 
        void save_parse_state() final {
            m_state_stack.push_back(m_state);
        }

        /**
         * Rejects the current parse state.
         * The parse state is restored from the last entry 
         * in the internal parse state stack.
         */
        void reject_parse_state() final {
            assert(!m_state_stack.empty());
            m_state = m_state_stack.back();
            m_state_stack.pop_back();
        }

        /**
         * Accepts the current parse state by removing the last entry
         * saved in the internal parse state stack.
         */
        void accept_parse_state() final {
            assert(!m_state_stack.empty());
            m_state_stack.pop_back();
        }

        /**
         * Saves the current match start state to an internal stack.
         * A match start state is not always the same as the current parse state,
         * due to left recursion.
         */
        void save_match_start_state() final {
            m_match_start_state_stack.push_back(m_match_start_state);
        }

        /**
         * Restores the current match start state from the internal stack.
         */
        void restore_match_start_state() final {
            assert(!m_match_start_state_stack.empty());
            m_match_start_state = m_match_start_state_stack.back();
            m_match_start_state_stack.pop_back();
        }

        /**
         * Adds a match with the given id.
         * The source range for the match is from the current match start state to the current parse state.
         * @param id the id of the match.
         */
        void add_match(int id) final {
            match_container_type children(m_matches.begin() + m_match_start_state.match_count, m_matches.end());
            m_matches.resize(m_match_start_state.match_count);
            m_matches.push_back(match_type(static_cast<match_id_type>(id), m_match_start_state.parse_position, m_state.parse_position, std::move(children)));
            m_state.match_count = m_matches.size();
            m_match_start_state.match_count = m_matches.size();
        }

        /**
         * Saves the current error start parse state into an internal stack.
         */
        void save_error_start_state() final {
            m_error_start_state_stack.push_back(m_error_start_state);
        }

        /**
         * Restores the error start state from the internal stack.
         */
        void restore_error_start_state() final {
            assert(!m_error_start_state_stack.empty());
            m_error_start_state = m_error_start_state_stack.back();
            m_error_start_state_stack.pop_back();
        }

        /**
         * Adds an error with the given id.
         * The source range for the error is from the current error start state to the current parse state.
         * @param id the id of the error.
         */
        void add_error(int id) final {
            m_errors.push_back(parse_error_type(static_cast<error_id_type>(id), m_error_start_state.parse_position, m_state.parse_position));
        }

        /**
         * Returns the current left recursion state for a parse node.
         * @param parse_node_id id of the parse node to get the left recursion state of.
         * @return a pair of:
         *  - the left recursion state of the parse node.
         *  - a flag which indicates if a parse node is left-recursive at the current parse state.
         */
        std::pair<left_recursion_state_type, bool> get_left_recursion_state(parse_node_id_type parse_node_id) const final {
            parse_node_state& pns = get_parse_node_state(parse_node_id);
            return { pns.state.type, m_state.parse_position.get_iterator() == pns.state.iterator };
        }

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'no_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        void begin_no_left_recursion_state(uintptr_t parse_node_id) final {
            begin_left_recursion_state(parse_node_id, left_recursion_state_type::no_left_recursion);
        }

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'reject_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        void begin_reject_left_recursion_state(uintptr_t parse_node_id) final {
            begin_left_recursion_state(parse_node_id, left_recursion_state_type::reject_left_recursion);
        }

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'accept_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        void begin_accept_left_recursion_state(uintptr_t parse_node_id) final {
            begin_left_recursion_state(parse_node_id, left_recursion_state_type::accept_left_recursion);
            m_state.end = m_state.parse_position.get_iterator();
        }

        /**
         * Sets the left recursion state type for a parse node to 'accepted_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        void accept_left_recursion_state(parse_node_id_type parse_node_id) final {
            parse_node_state& pns = get_parse_node_state(parse_node_id);
            assert(!pns.state_stack.empty());
            pns.state_stack.back().type = left_recursion_state_type::accepted_left_recursion;
            m_state.end = m_end_iterator;
        }

        /**
         * Restores the current parse node state for left recursion, from the internal stack,
         * for the parse node with the given id.
         * @param parse_node_id id of the parse node to restore the left recursion state of.
         */
        void restore_left_recursion_state(uintptr_t parse_node_id) final {
            parse_node_state& pns = get_parse_node_state(parse_node_id);
            assert(!pns.state_stack.empty());
            pns.state = pns.state_stack.back();
            pns.state_stack.pop_back();
        }

        /**
         * Returns the current parse position.
         * @return the current parse position.
         */
        const parse_position_type& get_parse_position() const {
            return m_state.parse_position;
        }

        /**
         * Returns the matches.
         * @return the matches.
         */
        const match_container_type& get_matches() const {
            return m_matches;
        }

        /**
         * Returns the errors.
         * @return the errors.
         */
        const parse_error_container_type& get_errors() const {
            return m_errors;
        }

    private:
        //parse state type
        struct state {
            parse_position_type parse_position;
            std::size_t match_count;
            iterator_type end;
        };

        //parse state stack type
        using state_stack = std::vector<state>;

        //match start state type
        struct match_start_state {
            parse_position_type parse_position;
            std::size_t match_count;
        };

        //match start state stack type
        using match_start_state_stack = std::vector<match_start_state>;

        //error start state type
        struct error_start_state {
            parse_position_type parse_position;
        };

        //error start state stack
        using error_start_state_stack = std::vector<error_start_state>;

        //left recursion state type
        struct left_recursion_state {
            iterator_type iterator;
            left_recursion_state_type type;
        };

        //left recursion state stack
        using left_recursion_state_stack = std::vector<left_recursion_state>;

        //parse node state for left recursion
        struct parse_node_state {
            left_recursion_state state;
            left_recursion_state_stack state_stack;
        };

        //parse node state map
        using parse_node_state_map = std::map<parse_node_id_type, parse_node_state>;

        //states
        state m_state;
        match_start_state m_match_start_state;
        error_start_state m_error_start_state;

        //state stacks
        state_stack m_state_stack;        
        match_start_state_stack m_match_start_state_stack;        
        error_start_state_stack m_error_start_state_stack;

        //state maps
        mutable parse_node_state_map m_parse_node_state_map;

        //output
        match_container_type m_matches;
        parse_error_container_type m_errors;

        //constants
        const parse_node_state m_initial_parse_node_state;
        const iterator_type m_end_iterator;

        //get the parse node state for the specific parse node id
        parse_node_state& get_parse_node_state(parse_node_id_type id) const {
            const auto [it, ok] = m_parse_node_state_map.insert(std::make_pair(id, m_initial_parse_node_state));
            return it->second;
        }

        //start a lefft recursion state for the specific parse node id
        void begin_left_recursion_state(uintptr_t parse_node_id, left_recursion_state_type type) {
            parse_node_state& pns = get_parse_node_state(parse_node_id);
            pns.state_stack.push_back(pns.state);
            pns.state.iterator = m_state.parse_position.get_iterator();
            pns.state.type = type;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_PARSE_CONTEXT_HPP
