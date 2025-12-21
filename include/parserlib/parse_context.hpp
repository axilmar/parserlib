#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include "match.hpp"
#include "parse_error.hpp"
#include "default_symbol_comparator.hpp"
#include "parse_iterator.hpp"


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
        using symbol_comparator_type = default_symbol_comparator;

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
             * The constructor.
             * @param begin begin iterator.
             * @param end end iterator.
             */
            state(const iterator_type& begin, const iterator_type& end)
                : m_parse_state(begin)
                , m_match_parse_state(begin)
                , m_end(end)
            {
            }

            /**
             * Returns the parse state.
             * @return the parse state.
             */
            const parse_state& get_parse_state() const noexcept {
                return m_parse_state;
            }

            /**
             * Returns the parse state for matches.
             * @return the parse state for matches.
             */
            const parse_state& get_match_parse_state() const noexcept {
                return m_match_parse_state();
            }

            /**
             * Returns the end iterator.
             * @return the end iterator.
             */
            const iterator_type& get_end() const {
                return m_end;
            }

        private:
            parse_state m_parse_state;
            parse_state m_match_parse_state;
            iterator_type m_end;

            friend parse_context_type;
        };

        /**
         * The constructor.
         * @param begin begin parse position.
         * @param end end parse position.
         */
        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state(begin, end)
            , m_end(end)
        {
        }

        /**
         * Constructor from container that has `begin()` and `end()` members.
         * @param src the container.
         */
        template <class Container>
        parse_context(Container& src)
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
            m_state.m_match_parse_state.m_iterator = ++m_state.m_parse_state.m_iterator;
        }

        /**
         * Increments the parse position by a count.
         * If the end of input is exceeded, the result is undefined.
         * @param count number of positions to increment the parse position.
         */
        void increment_parse_position(size_t count) {
            m_state.m_match_parse_state.m_iterator = m_state.m_parse_state.m_iterator += count;
        }

        /**
         * Increments the parse position line.
         */
        void increment_parse_position_line() {
            m_state.m_match_parse_state.m_iterator = m_state.m_parse_state.m_iterator.increment_line();
        }

        /**
         * Returns the parse iterator.
         * @return the parse iterator.
         */
        const iterator_type& get_iterator() const {
            return m_state.m_parse_state.m_iterator;
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
            m_matches.resize(s.m_parse_state.match_count);
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
         * Adds an error to the errors of this context.
         * @param id id of error.
         * @param begin_state state for the error start.
         * @param end_state state for the error end.
         */
        void add_error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) {
            m_errors.emplace_back(id, begin, end);
        }

        /**
         * Executes a function.
         * If the function fails, or an exception happens, 
         * then the state of this parse context is restored.
         * @param fn function; it is passed this parse context.
         * @return true if the function suceeds, false otherwise.
         */
        template <class F>
        bool do_and_restore_state_on_failure(const F& fn) {
            const state initial_state = m_state;
            try {
                if (fn()) {
                    return true;
                }
                m_state = initial_state;
            }
            catch (...) {
                m_state = initial_state;
                throw;
            }
            return false;
        }

        /**
         * Executes a function, then restores the state to the one before the function was called.
         * If an exception happens, then the state of this parse context is restored.
         * @param fn function; it is passed this parse context.
         * @return true if the function suceeds, false otherwise.
         */
        template <class F>
        bool do_and_restore_state(const F& fn) {
            const state initial_state = m_state;
            try {
                const bool result = fn();
                m_state = initial_state;
                return result;
            }
            catch (...) {
                m_state = initial_state;
                throw;
            }
        }

    private:
        state m_state;
        const iterator_type m_end;
        match_container_type m_matches;
        parse_error_container_type m_errors;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
