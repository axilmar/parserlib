#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <string>
#include "source_range.hpp"
#include "symbol_comparator.hpp"


namespace parserlib {


    /**
     * Class with parse state.
     * @param Iterator iterator to use.
     * @param MatchId type of match id.
     * @param ErrorId type of error id.
     */ 
    template <class Iterator = std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parse_context {
    public:
        /** The iterator type. */
        using iterator_type = Iterator;

        /** the match id type. */
        using match_id_type = MatchId;

        /** the match type. */
        using match_type = match<MatchId, Iterator>;

        /** the match container type. */
        using match_container_type = std::vector<match_type>;

        /** The symbol comparator type. */ 
        using symbol_comparator_type = SymbolComparator;

        /**
         * The match start state.
         */ 
        class match_start_state {
        private:
            Iterator m_iterator;
            size_t m_match_count;

            match_start_state(const Iterator& iterator, size_t match_count = 0)
                : m_iterator(iterator)
                , m_match_count(match_count)
            {
            }

            friend class parse_context;
        };

        /**
         * The parse context state.
         */ 
        class state {
        public:
            /**
             * The constructor.
             * @param begin the initial iterator.
             * @param end the end iterator.
             */ 
            state(const Iterator& begin, const Iterator& end) 
                : m_iterator(begin)
                , m_match_count(0)
                , m_end(end)
                , m_match_start_state(begin)
            {
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */ 
            const Iterator& get_iterator() const {
                return m_iterator;
            }

        private:
            Iterator m_iterator;
            size_t m_match_count;
            Iterator m_end;
            match_start_state m_match_start_state;

            friend class parse_context;
        };

        /**
         * Constructor from source range.
         * @param begin the begin iterator.
         * @param end the end iterator.
         */ 
        parse_context(const Iterator& begin, const Iterator& end)
            : m_state(begin, end)
        {
        }

        /**
         * Constructor from a container.
         * @param container the source container.
         */ 
        template <class Container>
        parse_context(const Container& container)
            : parse_context(container.begin(), container.end())
        {
        }

        /**
         * Returns the current state.
         * @return the current state.
         */ 
        const state& get_state() const {
            return m_state;
        }

        /**
         * Sets the current state.
         * @param s the state to set.
         */ 
        void set_state(const state& s) {
            m_state = s;
            m_matches.resize(m_state.m_match_count);
        }

        /**
         * Compares two symbols using the supplied symbol comparator.
         * @param left the left symbol.
         * @param right the right symbol.
         * @return their difference.
         */ 
        template <class L, class R>
        static int compare(const L& left, const R& right) {
            return SymbolComparator::compare(left, right);
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */ 
        const Iterator& get_end_iterator() const {
            return m_state.m_end;
        }

        /**
         * Returns the current iterator.
         * @return the current iterator.
         */ 
        const Iterator& get_iterator() const {
            return m_state.m_iterator;
        }

        /**
         * Checks if the end position is not reached.
         * @return true if the end position is not reached, false otherwise.
         */ 
        bool is_valid_iterator() const {
            return m_state.m_iterator != m_state.m_end;
        }

        /**
         * Checks if the end position is reached.
         * @return true if the end position is reached, false otherwise.
         */ 
        bool is_end_iterator() const {
            return m_state.m_iterator == m_state.m_end;
        }

        /**
         * Increments the iterator by one position.
         */ 
        void increment_iterator() {
            assert(is_valid_iterator());
            ++m_state.m_iterator;
            m_state.m_match_start_state.m_iterator = m_state.m_iterator;
        }

        /**
         * Increments the iterator by the specific count.
         * @param count the increment count.
         */ 
        void increment_iterator(size_t count) {
            assert(is_valid_iterator());
            m_state.m_iterator += count;
            m_state.m_match_start_state.m_iterator = m_state.m_iterator;
        }

        /**
         * Increments the line.
         * Valid only if the iterator supports it.
         */ 
        void increment_line() {
            m_state.m_iterator.increment_line();
        }

        /**
         * Returns the container of matches.
         * @return the container of matches.
         */ 
        const match_container_type& get_matches() const {
            return m_matches;
        }

        /**
         * Returns the current match start state.
         * @return the current match start state.
         */ 
        const match_start_state& get_match_start_state() const {
            return m_state.m_match_start_state;
        }

        /**
         * Adds a match.
         * The range of the match is from the provided state up to the current state.
         * @param id id.
         * @param from_state the initial state to start the match from.
         */ 
        void add_match(const MatchId& id, const match_start_state& from_state) {
            match_container_type children(m_matches.begin() + from_state.m_match_count, m_matches.end());
            m_matches.resize(from_state.m_match_count);
            m_matches.push_back(match_type(id, from_state.m_iterator, m_state.m_iterator, std::move(children)));
            m_state.m_match_count = m_matches.size();
            m_state.m_match_start_state.m_match_count = m_state.m_match_count;
        }

        /**
         * Creates a parse context that can be used to parse the matches of this parse context.
         * @return a new parse context that can be used to parse the matches of this parse context.
         */ 
        template <class DerivedMatchId = int, class DerivedErrorId = int, class DerivedSymbolComparator = default_symbol_comparator>
        parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>
        derive_parse_context() const {
            return m_matches;
        }

    private:
        state m_state;
        match_container_type m_matches;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
