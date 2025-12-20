#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include "match.hpp"
#include "parse_error.hpp"
#include "default_symbol_comparator.hpp"
#include "parse_iterator.hpp"


namespace parserlib {


    /**
     * A parse context holds the state of parsing/
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
        class state {
        public:
            /**
             * Iterator that points to the first unparsed position.
             * @return iterator points to the first unparsed position.
             */
            const iterator_type& begin() const noexcept {
                return m_begin;
            }

            /**
             * Iterator that points to the end position.
             * @return iterator points to the end position.
             */
            const iterator_type& end() const noexcept {
                return m_end;
            }

            /**
             * Returns number of matches at this state.
             * @return number of matches at this state.
             */
            size_t get_match_count() const noexcept {
                return m_match_count;
            }

        private:
            iterator_type m_begin;
            iterator_type m_end;
            size_t m_match_count;

            state(const iterator_type& begin, const iterator_type& end, size_t mc) 
                : m_begin(begin)
                , m_end(end)
                , m_match_count(mc)
            {
            }

            friend parse_context_type;
        };

        /**
         * The constructor.
         * @param begin begin parse position.
         * @param end end parse position.
         */
        parse_context(const iterator_type& begin, const iterator_type& end)
            : m_state(begin, end, 0)
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
            return m_state.m_begin != m_state.m_end;
        }

        /**
         * Returns the current symbol.
         * @return the current symbol.
         */
        auto get_current_symbol() const {
            return *m_state.m_begin;
        }

        /**
         * Compares symbols.
         * The operands must be convertible to `int`.
         * @param left the left operand.
         * @param right the right operand.
         * @return their numeric difference.
         */
        template <class L, class R>
        static int compare_symbols(const L& left, const R& right) noexcept {
            return symbol_comparator_type::compare(left, right);
        }

        /**
         * Increments the parse position by one.
         * If the end of input is exceeded, the result is undefined.
         */
        void increment_parse_position() {
            ++m_state.m_begin;
        }

        /**
         * Increments the parse position by a count.
         * If the end of input is exceeded, the result is undefined.
         * @param count number of positions to increment the parse position.
         */
        void increment_parse_position(size_t count) {
            m_state.m_begin += count;
        }

        /**
         * Increments the text position line.
         */
        void increment_text_position_line() {
            m_state.m_begin.increment_text_position_line();
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
         */
        void set_state(const state& s) {
            m_state = s;
            m_matches.resize(s.m_match_count);
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
         * @param end_state state for the match end.
         */
        void add_match(const match_id_type& id, const state& begin_state, const state& end_state) {
            match_container_type children{ m_matches.begin() + begin_state.m_match_count, m_matches.end() };
            m_matches.resize(begin_state.m_match_count);
            m_matches.emplace_back(id, begin_state.m_begin, end_state.m_begin, std::move(children));
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
        void add_error(const error_id_type& id, const state& begin_state, const state& end_state) {
            m_errors.emplace_back(id, begin_state.m_begin, end_state.m_begin);
        }

    private:
        state m_state;
        const iterator_type m_end;
        match_container_type m_matches;
        parse_error_container_type m_errors;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
