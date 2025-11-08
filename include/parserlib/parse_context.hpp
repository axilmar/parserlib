#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <map>
#include <algorithm>
#include "parse_context_data.hpp"
#include "parse_context_default_options.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    /**
     * The `parse_context` class contains a parser's state.
     * It is constructed from a range of symbols (i.e. a string, a set of tokens etc),
     * which must be statically castable to int (for comparisons).
     * It contains a parse position, which is manipulated by parser node classes.
     * It contains a set of matches over the source.
     * It contains state that allows rules to handle left recursion.
     *
     * @param Source the source type; it can be any STL-like container.
     * @param MatchId type of id for matches; it can be any custom enumeration or any other type.
     * @param ErrorId type of id for errors; it can be any custom enumeration or any other type.
     * @param TextPosition position in text; it is optionally used for providing line and column information.
     * @param SymbolComparator class used for comparing symbols; it can be used for case-insensitive comparisons.
     * @param Extensions the list of parse context extensions.
     */
    template <
        class Source = default_source_type,
        class MatchId = default_match_id_type,
        class ErrorId = default_error_id_type,
        class TextPosition = default_text_position_type,
        class SymbolComparator = default_symbol_comparator_type,
        class... Extensions
    >
    class parse_context : public Extensions... {
    public:
        /** The source type. */
        using source_type = Source;

        /** The iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** The match id type. */
        using match_id_type = MatchId;

        /** The error id type. */
        using error_id_type = ErrorId;

        /** The text position type. */
        using text_position_type = TextPosition;

        /** The symbol comparator type. */
        using symbol_comparator_type = SymbolComparator;

        /** The parse position type. */
        using parse_position_type = parse_position<source_type, text_position_type>;

        /** The match type. */
        using match_type = match<source_type, match_id_type, text_position_type>;

        /** The match container type. */
        using match_container_type = std::vector<match_type>;

        /** The error type. */
        using error_type = parse_error<source_type, match_id_type, text_position_type>;

        /** The error container type. */
        using error_container_type = std::vector<error_type>;

        /** This class' type. */
        using parse_context_type = parse_context<source_type, match_id_type, error_id_type, text_position_type, symbol_comparator_type, Extensions...>;

        /** Rule type. */
        using rule_type = rule<parse_context_type>;

        /**
         * Internal state type.
         */
        class state {
        public:
            /**
             * Returns the parse position.
             * @return the parse position.
             */
            const parse_position_type& parse_position() const {
                return m_parse_position;
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */
            const iterator_type& iterator() const {
                return m_parse_position.iterator();
            }

            /**
             * Returns the text position.
             * @return the text position.
             */
            const text_position_type& text_position() const {
                return m_parse_position.text_position();
            }

            /**
             * Returns the match count.
             * @return the match count.
             */
            size_t match_count() const {
                return m_match_count;
            }

            /**
             * Returns the error count.
             * @return the error count.
             */
            size_t error_count() const {
                return m_error_count;
            }

        private:
            parse_position_type m_parse_position;
            size_t m_match_count;
            size_t m_error_count;
            bool m_terminal_parsing_allowed;

            state(const parse_position_type& parse_pos, size_t match_count = 0, size_t error_count = 0, bool tpa = false)
                : m_parse_position(parse_pos)
                , m_match_count(match_count)
                , m_error_count(error_count)
                , m_terminal_parsing_allowed(tpa)
            {
            }
            friend parse_context_type;
        };

        /**
         * Constructor from source range.
         * @param begin start of source.
         * @param end end of source.
         */
        parse_context(const iterator_type& begin, const iterator_type& end)            
            : m_parse_position(begin)
            , m_end_iterator(end)
            , m_left_recursion_start_state(begin)
            , m_begin_iterator(begin)
        {
        }

        /**
         * Constructor from source.
         * @param source the source.
         */
        parse_context(source_type& source)
            : parse_context(source.begin(), source.end())
        {
        }

        /**
         * Constructor from source range and extension args.
         * @param begin start of source.
         * @param end end of source.
         * @param extensionArgs arguments for the extensions.
         */
        template <class... ExtensionArgs>
        parse_context(const iterator_type& begin, const iterator_type& end, ExtensionArgs&&... extensionArgs)
            : Extensions(std::forward<ExtensionArgs>(extensionArgs))...
            , m_parse_position(begin)
            , m_end_iterator(end)
            , m_left_recursion_start_state(begin)
            , m_begin_iterator(begin)
        {
        }

        /**
         * Constructor from source and extension args.
         * @param source the source.
         * @param extensionArgs arguments for the extensions.
         */
        template <class... ExtensionArgs>
        parse_context(source_type& source, ExtensionArgs&&... extensionArgs)
            : parse_context(source.begin(), source.end(), std::forward<ExtensionArgs>(extensionArgs)...)
        {
        }

        /**
         * Returns the current parse position.
         * @return the current parse position.
         */
        const parse_position_type& parse_position() const {
            return m_parse_position;
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& iterator() const {
            return m_parse_position.iterator();
        }

        /**
         * Returns the current text position.
         * @return the current text position.
         */
        const text_position_type& text_position() const {
            return m_parse_position.text_position();
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */
        const iterator_type& begin_iterator() const {
            return m_begin_iterator;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const iterator_type& end_iterator() const {
            return m_end_iterator;
        }

        /**
         * Checks if the end of the source is reached.
         * @return true if the end of the source has not been reached, false otherwise.
         */
        bool parse_valid() const {
            return m_parse_position.iterator() != m_end_iterator;
        }

        /**
         * Checks if the end of the source is reached.
         * @return true if the end of the source has been reached, false otherwise.
         */
        bool parse_ended() const {
            return m_parse_position.iterator() == m_end_iterator;
        }

        /**
         * Increments the parse position (iterator and text position) by one.
         */
        void increment_parse_position() {
            assert(parse_valid());
            m_parse_position.increment();
        }

        /**
         * Increments the parse position (iterator and text position column) by a specific count.
         * @param count number of columns to add to the current parse position.
         */
        void increment_parse_position(size_t count) {
            assert(parse_valid());
            assert(std::next(m_parse_position.iterator(), count) <= m_end_iterator);
            m_parse_position.increment(count);
        }

        /**
         * Increments the text position line.
         * It does not increment any iterators.
         * The text column is set to its initial value.
         */
        void increment_parse_position_line() {
            m_parse_position.increment_line();
        }

        /**
         * Returns the match count.
         * @return the match count.
         */
        size_t match_count() const {
            return m_matches.size();
        }

        /**
         * Returns the matches currently available.
         * @return the matches currently available.
         */
        const match_container_type& matches() const {
            return m_matches;
        }

        /**
         * Returns the current state for a match start.
         * @return the state for a match start.
         */
        state get_match_start_state() const {
            return terminal_parsing_allowed() ? get_state() : m_left_recursion_start_state;
        }

        /**
         * Adds a match.
         * The range is from the given start state's position, up to the current iterator.
         * @param id match id.
         * @param start_state start state.
         * @param end_iterator end iterator.
         */
        void add_match(const match_id_type& id, const state& start_state, const iterator_type& end_iterator) {
            match_container_type children(std::next(m_matches.begin(), start_state.m_match_count), m_matches.end());
            m_matches.resize(start_state.m_match_count);
            m_matches.push_back(match_type(id, start_state.m_parse_position, end_iterator, std::move(children)));
        }

        /**
         * Returns the error count.
         * @return the error count.
         */
        size_t error_count() const {
            return m_errors.size();
        }

        /**
         * Returns the errors currently available.
         * @return the errors currently available.
         */
        const error_container_type& errors() const {
            return m_errors;
        }

        /**
         * Adds an error.
         * The error will be cancelled if a higher node restores the parse context to a previous state.
         * @param id error id.
         * @param start_pos start position.
         * @param end_iterator end iterator.
         */
        void add_error(const error_id_type& id, const parse_position_type& start_pos, const iterator_type& end_iterator) {
            m_errors.push_back(error_type(id, start_pos, end_iterator));
        }

        /**
         * Returns the current state of the parse context.
         * The state can be used to rewind the parse context to a prior state.
         * @return the current state of the parse context.
         */
        state get_state() const {
            return state(m_parse_position, m_matches.size(), m_errors.size(), m_terminal_parsing_allowed);
        }

        /**
         * Sets the current state of the parse context.
         * @param st the state to set.
         */
        void set_state(const state& st) {
            m_parse_position = st.m_parse_position;
            m_matches.resize(st.m_match_count);
            m_errors.resize(st.m_error_count);
            m_terminal_parsing_allowed = st.m_terminal_parsing_allowed;
        }

        /**
         * Using the supplied symbol comparator type, it compares two symbols.
         * @param a the first symbol.
         * @param b the second symbol.
         * @return -1, 0 or 1 depending on if the first symbol is less then, equal to or greater than the second symbol value.
         */
        template <class A, class B>
        int compare_symbols(const A& a, const B& b) const {
            return m_symbol_comparator(static_cast<int>(a), static_cast<int>(b));
        }

        /**
         * Using the supplied symbol comparator type, it compares the current symbol
         * with the given value.
         * @param other the other value to compare the current symbol to.
         * @return -1, 0 or 1 depending on if the current symbol is less then, equal to or greater than the other symbol value.
         */
        template <class T>
        int compare_current_symbol(const T& other) const {
            assert(parse_valid());
            return compare_symbols(*m_parse_position.iterator(), other);
        }

        /**
         * Checks if a terminal should or should not be parsed at the current parse context state.
         * Used in support of left recursion parsing, in which there is a state where terminals
         * shall not be parsed.
         * @return true if a terminal parsing can be parsed at this state, false otherwise.
         */
        bool terminal_parsing_allowed() const {
            return m_terminal_parsing_allowed;
        }

        /**
         * Returns the current left recursion start state.
         * @return the current left recursion start state.
         */
        const state& left_recursion_start_state() const {
            return m_left_recursion_start_state;
        }

        /**
         * Sets the left recursion start state.
         * @param st the new left recursion start state.
         */
        void set_left_recursion_start_state(const state& st) {
            m_left_recursion_start_state = st;
        }

        /**
         * Returns a parse context for the matches found while using this parse context.
         * Provided so as that it can be used in multi-stage parsing.
         * @param MatchId match id type.
         * @param ErrorId error id type.
         * @param Extensions parse context extensions.
         */
        template <class DerivedMatchId, class DerivedErrorId, class... DerivedExtensions>
        auto derive_parse_context() {
            return parse_context<match_container_type, DerivedMatchId, DerivedErrorId, default_text_position, default_symbol_comparator, DerivedExtensions...>(m_matches);
        }

    private:
        enum class rule_state {
            none,
            reject,
            accept
        };

        struct rule_data {
            iterator_type iterator;
            rule_state state;
        };

        parse_position_type m_parse_position;
        const iterator_type m_end_iterator;
        match_container_type m_matches;
        symbol_comparator_type m_symbol_comparator;
        state m_left_recursion_start_state;
        bool m_terminal_parsing_allowed{ true };
        std::map<const rule_type*, rule_data> m_rule_data;
        const iterator_type m_begin_iterator;
        error_container_type m_errors;

        friend rule_type;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
