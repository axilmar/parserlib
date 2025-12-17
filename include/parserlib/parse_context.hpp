#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <vector>
#include <utility>
#include <cstddef>
#include "parse_node_id_type.hpp"
#include "left_recursion_state_type.hpp"


namespace parserlib {


    /**
     * Base class for parse contexts.
     * It provides the interfaces needed for parsing.
     * 
     * The APIs provided are about:
     * 
     *  - managing the parse position.
     *  - doing symbol comparisons.
     *  - providing the basic parse operations for performance (parsing symbols, sequences, sets, ranges, any, etc...).
     *  - managing the parse state.
     *  - managing matches.
     *  - managing errors.
     *  - managing left recursion.
     */
    class parse_context {
    public:
        /**
         * Destructor.
         * Virtual due to polymorphism.
         */
        virtual ~parse_context() {
        }

        /**
         * Tests whether the end position is reached or not.
         * @return true if the end position is not reached, false otherwise.
         */
        virtual bool is_valid_parse_position() const = 0;

        /**
         * Tests whether the end position is reached or not.
         * @return true if the end position is reached, false otherwise.
         */
        bool is_end_parse_position() const {
            return !is_valid_parse_position();
        }

        /**
         * Increments the current parse position by one symbol.
         */
        virtual void increment_parse_position() = 0;

        /**
         * Increments the current parse position by a count.
         * @param count number of symbols to increment the parse position.
         */
        virtual void increment_parse_position(std::size_t count) = 0;

        /**
         * Increments the line of the current parse position.
         */
        virtual void increment_parse_position_line() = 0;

        /**
         * Retrieves the current symbol, i.e. the symbol at the current parse position.
         * @return the current symbol.
         */
        virtual int get_current_symbol() const = 0;

        /**
         * Compares two symbols.
         * @param left the left symbol to compare.
         * @param right the right symbol to compare.
         * @return less than 0 if left < right, 0 if left == right, greater than 0 if left > right.
         */
        virtual int compare_symbols(int left, int right) const = 0;

        /**
         * Parses a single symbol.
         * @param symbol symbol to compare.
         * @return true on success, false on failure.
         */
        virtual bool parse_symbol(int symbol) = 0;

        /**
         * Parses the given symbol sequence.
         * @param sequence the sequence of symbols to compare.
         * @return true on success, false on failure.
         */
        virtual bool parse_symbol_sequence(const std::vector<int>& sequence) = 0;

        /**
         * Parses the symbol at the current parse position out of a set of symbols.
         * @param set the set of symbols to compare.
         * @return true if the current symbol is within the given set, false otherwise.
         */
        virtual bool parse_symbol_set(const std::vector<int>& set) = 0;

        /**
         * Parses the symbol at the current parse position within a symbol range.
         * @param min min symbol value.
         * @param max max symbol value.
         * @return true if the current symbol is within the given range, false otherwise.
         */
        virtual bool parse_symbol_range(int min, int max) = 0;

        /**
         * Parses any symbol, except if the end of input has been reached.
         * @return true if the end of input has not been reached, false otherwise.
         */
        virtual bool parse_any_symbol() = 0;

        /**
         * Saves the current parse state into an internal stack.
         */ 
        virtual void save_parse_state() = 0;

        /**
         * Rejects the current parse state.
         * The parse state is restored from the last entry 
         * in the internal parse state stack.
         */
        virtual void reject_parse_state() = 0;

        /**
         * Accepts the current parse state by removing the last entry
         * saved in the internal parse state stack.
         */
        virtual void accept_parse_state() = 0;

        /**
         * Saves the current match start state to an internal stack.
         * A match start state is not always the same as the current parse state,
         * due to left recursion.
         */
        virtual void save_match_start_state() = 0;

        /**
         * Restores the current match start state from the internal stack.
         */
        virtual void restore_match_start_state() = 0;

        /**
         * Adds a match with the given id.
         * The source range for the match is from the current match start state to the current parse state.
         * @param id the id of the match.
         */
        virtual void add_match(int id) = 0;

        /**
         * Saves the current error start parse state into an internal stack.
         */
        virtual void save_error_start_state() = 0;

        /**
         * Restores the error start state from the internal stack.
         */
        virtual void restore_error_start_state() = 0;

        /**
         * Adds an error with the given id.
         * The source range for the error is from the current error start state to the current parse state.
         * @param id the id of the error.
         */
        virtual void add_error(int id) = 0;

        /**
         * Returns the current left recursion state for a parse node.
         * @param parse_node_id id of the parse node to get the left recursion state of.
         * @return a pair of:
         *  - the left recursion state of the parse node.
         *  - a flag which indicates if a parse node is left-recursive at the current parse state.
         */
        virtual std::pair<left_recursion_state_type, bool> get_left_recursion_state(parse_node_id_type parse_node_id) const = 0;

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'no_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        virtual void begin_no_left_recursion_state(uintptr_t parse_node_id) = 0;

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'reject_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        virtual void begin_reject_left_recursion_state(uintptr_t parse_node_id) = 0;

        /**
         * Saves the current left recursion state for the parse node with the given id 
         * and sets its left recursion state type to 'accept_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        virtual void begin_accept_left_recursion_state(uintptr_t parse_node_id) = 0;

        /**
         * Sets the left recursion state type for a parse node to 'accepted_left_recursion'.
         * @param parse_node_id id of the parse node to set the left recursion state of.
         */
        virtual void accept_left_recursion_state(parse_node_id_type parse_node_id) = 0;

        /**
         * Restores the current parse node state for left recursion, from the internal stack,
         * for the parse node with the given id.
         * @param parse_node_id id of the parse node to restore the left recursion state of.
         */
        virtual void restore_left_recursion_state(uintptr_t parse_node_id) = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
