#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <map>
#include <unordered_map>
#include <algorithm>
#include "match.hpp"
#include "parse_error.hpp"
#include "default_symbol_comparator.hpp"
#include "parse_iterator.hpp"
#include "left_recursion_status.hpp"


namespace parserlib {


    /**
     * A parse context holds the state of parsing
     * and provides methods for retrieving and updating its state.
     * @param Iterator iterator type.
     * @param MatchId match id type.
     * @param ErrorId error id type.
     * @param SymbolComparator symbol comparator type.
     */
    template <
        class Iterator = typename std::string::const_iterator,
        class MatchId = int,
        class ErrorId = int,
        class SymbolComparator = default_symbol_comparator
    >
    class parse_context {
    public:
        /** Iterator type. */
        using iterator_type = Iterator;

        /** Match id type. */
        using match_id_type = MatchId;

        /** Error id type. */
        using error_id_type = ErrorId;

        /** Symbol comparator type. */
        using symbol_comparator_type = SymbolComparator;

        /** Parse context type. */
        using parse_context_type = parse_context<Iterator, MatchId, ErrorId, SymbolComparator>;

        /** Match type. */
        using match_type = match<match_id_type, iterator_type>;

        /** Match container type. */
        using match_container_type = std::vector<match_type>;

        /** Parse error type. */
        using parse_error_type = parse_error<error_id_type, iterator_type>;

        /** Parse error container type. */
        using parse_error_container_type = std::vector<parse_error_type>;

        /**
         * Type of derived parse context.
         * @param DerivedMatchId match id of the derived parse context.
         * @param DerivedErrorId error id of the derived parse context.
         * @param DerivedSymbolComparator symbol comparator for the derived parse context.
         */
        template <class DerivedMatchId = MatchId, class DerivedErrorId = ErrorId, class DerivedSymbolComparator = default_symbol_comparator>
        using derived_parse_context_type = parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>;

        /**
         * Parse state.
         */
        class parse_state {
        public:
            /**
             * The constructor.
             * @param iterator iterator.
             * @param match_count match count.
             */
            parse_state(const iterator_type& iterator, size_t match_count = 0)
                : m_iterator(iterator)
                , m_match_count(match_count)
            {
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */
            const iterator_type& get_iterator() const noexcept {
                return m_iterator;
            }

            /**
             * Returns the match count.
             * @return the match count.
             */
            size_t get_match_count() const noexcept {
                return m_match_count;
            }

        private:
            iterator_type m_iterator;
            size_t m_match_count;

            friend parse_context_type;
        };

        /**
         * Parse context state.
         */
        class state {
        public:
            /**
             * Returns the parse state.
             * @return the parse state.
             */
            const parse_state& get_parse_state() const noexcept {
                return m_parse_state;
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */
            const iterator_type& get_iterator() const {
                return m_parse_state.get_iterator();
            }

            /**
             * Returns the parse state for matches.
             * @return the parse state for matches.
             */
            const parse_state& get_match_parse_state() const noexcept {
                return m_match_parse_state;
            }

            /**
             * Returns the end iterator.
             * @return the end iterator.
             */
            const iterator_type& get_end_iterator() const {
                return m_end;
            }

            /**
             * Returns the number of errors at this particular state.
             * @return the number of errors at this particular state.
             */
            size_t get_error_count() const {
                return m_error_count;
            }

        private:
            parse_state m_parse_state;
            parse_state m_match_parse_state;
            iterator_type m_end;
            size_t m_error_count;

            state(const iterator_type& begin = iterator_type(), const iterator_type& end = iterator_type(), size_t error_count = 0)
                : m_parse_state(begin)
                , m_match_parse_state(begin)
                , m_end(end)
                , m_error_count(error_count)
            {
            }

            friend parse_context_type;
        };

        /**
         * Left recursion state.
         */
        class left_recursion_state {
        public:
            /**
             * The constructor.
             * @param it the iterator.
             * @param st the status.
             * @param initial the initial status; initial state is invalid.
             */
            left_recursion_state(const iterator_type& it, left_recursion_status st, bool initial = false)
                : m_iterator(it)
                , m_status(st)
                , m_initial_state(initial)
            {
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */
            const iterator_type& get_iterator() const {
                return m_iterator;
            }

            /**
             * Returns the status.
             * @return the status.
             */
            left_recursion_status get_status() const {
                return m_status;
            }

            /**
             * Checks if this state is the initial one.
             * @return true if the state is the initial one, false otherwise.
             */
            bool is_initial_state() const {
                return m_initial_state;
            }

        private:
            iterator_type m_iterator;
            left_recursion_status m_status;
            bool m_initial_state;
        };

        /**
         * The constructor.
         * @param begin begin parse position.
         * @param end end parse position.
         */
        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state(begin, end)
            , m_end(end)
            , m_initial_left_recursion_state(begin, left_recursion_status::no_left_recursion, true)
        {
        }

        /**
         * Constructor from container that has `begin()` and `end()` members.
         * @param src the container.
         */
        template <class Container>
        parse_context(const Container& src)
            : parse_context(src.begin(), src.end())
        {
        }

        /**
         * Checks if the current parse position is valid,
         * i.e. if the end has been reached.
         * @return true if the end position has not been reached yet, false otherwise.
         */
        bool is_valid_parse_position() const {
            return m_state.m_parse_state.m_iterator != m_state.m_end;
        }

        /**
         * Checks if the current parse position is the end.
         * @return true if the end position has been reached, false otherwise.
         */
        bool is_end_parse_position() const {
            return m_state.m_parse_state.m_iterator == m_state.m_end;
        }

        /**
         * Increments the parse position by one.
         * If the end of input is exceeded, the result is undefined.
         */
        void increment_parse_position() {
            ++m_state.m_parse_state.m_iterator;
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        /**
         * Increments the parse position by a count.
         * If the end of input is exceeded, the result is undefined.
         * @param count number of positions to increment the parse position.
         */
        void increment_parse_position(size_t count) {
            m_state.m_parse_state.m_iterator += count;
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        /**
         * Increments the parse position line.
         */
        void increment_parse_position_line() {
            m_state.m_parse_state.m_iterator.increment_line();
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        /**
         * Returns the parse iterator.
         * @return the parse iterator.
         */
        const iterator_type& get_iterator() const {
            return m_state.m_parse_state.m_iterator;
        }

        /**
            * Returns the end iterator.
            * @return the end iterator.
            */
        const iterator_type& get_end_iterator() const {
            return m_end;
        }

        /**
         * Returns the current symbol.
         * @return the current symbol.
         */
        auto get_symbol() const {
            return *m_state.m_parse_state.m_iterator;
        }

        /**
         * Compares symbols.
         * The operands must be convertible to `int`.
         * @param left the left operand.
         * @param right the right operand.
         * @return their numeric difference.
         */
        template <class L, class R>
        int compare_symbols(const L& left, const R& right) const noexcept {
            return symbol_comparator_type::compare(left, right);
        }

        /**
         * Returns the current state of the parse context.
         * @return the current state.
         */
        const state& get_state() const noexcept {
            return m_state;
        }

        /**
         * Restores the state of the parse context.
         * @param s the parse context state.
         */
        void set_state(const state& s) {
            m_state = s;
            m_matches.resize(s.m_parse_state.m_match_count);
            m_errors.resize(s.m_error_count);
        }

        /**
         * Returns the parse state for matches.
         * @return the parse state for matches.
         */
        const parse_state& get_match_parse_state() const noexcept {
            return m_state.m_match_parse_state;
        }

        /**
         * Sets the parse state for matches.
         * @param s the new match parse state.
         */
        void set_match_parse_state(const parse_state& s) {
            m_state.m_match_parse_state = s;
        }

        /**
         * Blocks parsing.
         */
        void block_parsing() {
            m_state.m_end = m_state.m_parse_state.m_iterator;
        }

        /**
         * Unblocks parsing.
         */
        void unblock_parsing() {
            m_state.m_end = m_end;
        }

        /**
         * Returns the current matches.
         * @return the current matches.
         */
        const match_container_type& get_matches() const noexcept {
            return m_matches;
        }

        /**
         * Adds a match to the matches of this context.
         * @param id id of match.
         * @param begin_state state for the match start.
         * @param end iterator for the match end.
         */
        void add_match(const match_id_type& id, const parse_state& begin_state, const iterator_type& end) {
            match_container_type children{ m_matches.begin() + begin_state.m_match_count, m_matches.end() };
            m_matches.resize(begin_state.m_match_count);
            m_matches.emplace_back(id, begin_state.m_iterator, end, std::move(children));
            m_state.m_match_parse_state.m_match_count = m_state.m_parse_state.m_match_count = m_matches.size();
        }

        /**
         * Returns the current errors.
         * @return the current errors.
         */
        const parse_error_container_type& get_errors() const noexcept {
            return m_errors;
        }

        /**
         * Sets the error count.
         * @param count the new error count.
         */
        void set_error_count(size_t count) {
            m_errors.resize(count);
        }

        /**
         * Adds an error to the errors of this context.
         * @param id id of error.
         * @param begin_state state for the error start.
         * @param end_state state for the error end.
         */
        void add_error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) {
            m_errors.emplace_back(id, begin, end);
            m_state.m_error_count = m_errors.size();
        }

        /**
         * Returns the left recursion state for the specific parse node.
         * @param pn address of parse node; used only as a key to a map.
         * @return the left recursion state for this parse node.
         */
        left_recursion_state& get_left_recursion_state(const void* pn) {
            const auto [it, ok] = m_left_recursion_states.insert(std::make_pair(pn, m_initial_left_recursion_state));
            return it->second;
        }

        /**
         * Creates a parse context for the matches.
         * @param DerivedMatchId the match id type for the derived parse context.
         * @param DerivedErrorId the error id type for the derived parse context.
         * @param DerivedSymbolComparator the error id type for the derived parse context.
         * @return a parse context that can be used to parse the matches of this parse context.
         */
        template <class DerivedMatchId = MatchId, class DerivedErrorId = ErrorId, class DerivedSymbolComparator = default_symbol_comparator>
        auto derive_parse_context() {
            return parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>(m_matches.begin(), m_matches.end());
        }

        /**
         * Invokes a parse node with memoization.
         * If the given parse node has been previously invoked at the same parse context state,
         * then the results of the parsing are restored from memory.
         * @param parse_node the parse node to parse.
         * @return true on success, false on failure.
         */
        template <class ParseNode>
        bool parse_with_memoization(const ParseNode& parse_node) {
            bool result;
            const void* key = &*m_state.m_parse_state.m_iterator;
            const auto it = m_memoized_states.find(key);
            if (it == m_memoized_states.end()) {
                const size_t prev_matches_size = m_matches.size();
                const size_t prev_errors_size = m_errors.size();
                result = parse_node.parse(*this);
                memoization_state& ms = m_memoized_states[key];
                ms.result_state = m_state;
                ms.result_matches.insert(ms.result_matches.end(), m_matches.begin() + prev_matches_size, m_matches.end());
                ms.result_errors.insert(ms.result_errors.end(), m_errors.begin() + prev_errors_size, m_errors.end());
                ms.result = result;
            }
            else {
                const memoization_state& ms = it->second;
                m_state = ms.result_state;
                m_matches.insert(m_matches.end(), ms.result_matches.begin(), ms.result_matches.end());
                m_errors.insert(m_errors.end(), ms.result_errors.begin(), ms.result_errors.end());
                result = ms.result;
            }
            return result;
        }

    private:
        struct memoization_state {
            state result_state;
            match_container_type result_matches;
            parse_error_container_type result_errors;
            bool result;
        };

        state m_state;
        match_container_type m_matches;
        parse_error_container_type m_errors;
        std::map<const void*, left_recursion_state> m_left_recursion_states;
        std::unordered_map<const void*, memoization_state> m_memoized_states;
        const iterator_type m_end;
        const left_recursion_state m_initial_left_recursion_state;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
