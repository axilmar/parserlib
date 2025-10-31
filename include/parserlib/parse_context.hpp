#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <string>
#include <cctype>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include "is_char.hpp"


namespace parserlib {


    /**
     * The default text position is empty.
     * It does not provide any line and column counting.
     */
    class default_text_position {
    public:
        /**
         * Returns the line.
         * @return always 0.
         */
        size_t line() const {
            return 0;
        }

        /**
         * Returns the column.
         * @return always 0.
         */
        size_t column() const {
            return 0;
        }

        /**
         * Increments the column by one.
         * Does nothing.
         */
        void increment_column() {
        }

        /**
         * Increments the column by a specific amount.
         * Does nothing.
         * @param count count to increment the column by.
         */
        void increment_column(size_t count) {
        }

        /**
         * Increments the line and sets the column to its initial value.
         * Does nothing.
         */
        void increment_line() {
        }

        /**
         * Returns an empty string,
         * since the default text position does not keep line and column.
         * @return an empty string.
         */
        std::string to_string() const {
            return {};
        }
    };


    /**
     * A text position object that counts lines and columns.
     * Both values start from 1, in order to match text editors.
     */
    class text_position {
    public:
        /**
         * Returns the line.
         * @return the line.
         */
        size_t line() const {
            return m_line;
        }

        /**
         * Returns the column.
         * @return the column.
         */
        size_t column() const {
            return m_column;
        }

        /**
         * Increments the column.
         */
        void increment_column() {
            ++m_column;
        }

        /**
         * Increments the column by a specific amount of positions.
         * @param count number of positions to increment the column.
         */
        void increment_column(size_t count) {
            m_column += count;
        }

        /**
         * Increments the line and sets the column to its initial value.
         */
        void increment_line() {
            ++m_line;
            m_column = 1;
        }

        /**
         * Returns the string "line L, column C"
         * @return a string.
         */
        std::string to_string() const {
            std::stringstream stream;
            stream << "line " << m_line << ", column " << m_column;
            return stream.str();
        }

    private:
        size_t m_line{ 1 };
        size_t m_column{ 1 };
    };


    /**
     * The default symbol comparator returns the difference between two operands, 
     * without any changes in the values.
     */
    class default_symbol_comparator {
    public:
        /**
         * Returns a - b without any conversion.
         * @param a the first operand.
         * @param b the second operand.
         * @return their difference.
         */
        int operator ()(int a, int b) const {
            return a - b;
        }
    };


    /**
     * The case insensitive symbol comparator returns the difference between two operands,
     * by converting both operands to lower case, using std::tolower.
     */
    class case_insensitive_symbol_comparator {
    public:
        /**
         * Returns std::tolower(a) - std::tolower(b).
         * @param a the first operand.
         * @param b the second operand.
         * @return their difference.
         */
        int operator ()(int a, int b) const {
            return std::tolower(a) - std::tolower(b);
        }
    };


    /**
     * A parse position.
     * It combines an iterator and a text position.
     * @param Source the source type.
     * @param TextPositon text position type.
     */
    template <class Source, class TextPosition> 
    class parse_position {
    public:
        /** The source type. */
        using source_type = Source;

        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** The text position type. */
        using text_position_type = TextPosition;

        /**
         * The constructor.
         * @param it iterator.
         * @param tpos the text position.
         */
        parse_position(const iterator_type& it = iterator_type(), const text_position_type& tpos = text_position_type())
            : m_iterator(it)
            , m_text_position(tpos)
        {
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& iterator() const {
            return m_iterator;
        }

        /**
         * Returns the text position.
         * @return the text position.
         */
        const text_position_type& text_position() const {
            return m_text_position;
        }

        /**
         * Increments the iterator and the text column by one.
         */
        void increment() {
            ++m_iterator;
            m_text_position.increment_column();
        }

        /**
         * Increments the iterator and the text column by a specific count.
         */
        void increment(size_t count) {
            m_iterator += count;
            m_text_position.increment_column(count);
        }

        /**
         * Increments the the text position line only by one.
         * The iterator is not affected.
         * The column is set to its initial value.
         */
        void increment_line() {
            m_text_position.increment_line();
        }

        /**
         * Converts the parse position to a string.
         * @param begin begin iterator; used in deriving the index of the parse position iterator,
         *  if the text position returns an empty string.
         * @return a string for this parse position.
         */
        std::string to_string(const iterator_type& begin) const {
            std::string result = m_text_position.to_string();
            if (result.empty()) {
                std::stringstream stream;
                const auto index = std::distance(begin, m_iterator);
                stream << "index " << index;
                result = stream.str();
            }
            return result;
        }

    private:
        iterator_type m_iterator;
        text_position_type m_text_position;
    };


    /**
     * A match represents a recognized portion of a source.
     * @param Source the source type.
     * @param MatchId match id type.
     * @param TextPosition text position type.
     */
    template <class Source, class MatchId, class TextPosition>
    class match {
    public:
        /** Source type. */
        using source_type = Source;

        /** Symbol type. */
        using symbol_type = typename Source::value_type;

        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** Match id type. */
        using match_id_type = MatchId;

        /** Text position type. */
        using text_position_type = TextPosition;

        /** Parse position type. */
        using parse_position_type = parse_position<source_type, text_position_type>;

        /** This class' type. */
        using match_type = match<source_type, match_id_type, text_position_type>;

        /** The match container type. */
        using match_container_type = std::vector<match_type>;

        /**
         * The constructor.
         * @param id match id.
         * @param start_pos start position.
         * @param end_it end_iterator.
         */
        match(
            const match_id_type& id = match_id_type(), 
            const parse_position_type& start_pos = parse_position_type(), 
            const iterator_type& end_it = iterator_type(), 
            match_container_type&& children = match_container_type()
        )
            : m_id(id)
            , m_start_position(start_pos)
            , m_end_iterator(end_it)
            , m_children(std::move(children))
        {
            assert(static_cast<intptr_t>(std::distance(m_start_position.iterator(), m_end_iterator)) >= 0);
        }

        /**
         * Returns the match id.
         * @return the match id.
         */
        const match_id_type& id() const {
            return m_id;
        }

        /**
         * Returns the start position.
         * @return the start position.
         */
        const parse_position_type& start_position() const {
            return m_start_position;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const iterator_type& end_iterator() const {
            return m_end_iterator;
        }

        /**
         * Returns the child matches.
         * @return the child matches.
         */
        const match_container_type& children() const {
            return m_children;
        }

        /**
         * Returns the part of the source that corresponds to this match.
         * @return either an instance of std::basic_string, if the source element is a character,
         *  or an instance of std::vector.
         */
        auto source() const {
            if constexpr (is_char_v<symbol_type>) {
                return std::basic_string<symbol_type>(m_start_position.iterator(), m_end_iterator);
            }
            else {
                return std::vector<symbol_type>(m_start_position.iterator(), m_end_iterator);
            }
        }

    private:
        match_id_type m_id;
        parse_position_type m_start_position;
        iterator_type m_end_iterator;
        match_container_type m_children;
    };


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
     * @param TextPosition position in text; it is optionally used for providing line and column information.
     * @param SymbolComparator class used for comparing symbols; it can be used for case-insensitive comparisons.
     */
    template <
        class Source = std::string,
        class MatchId = int,
        class TextPosition = default_text_position,
        class SymbolComparator = default_symbol_comparator
    >
    class parse_context {
    public:
        /** The source type. */
        using source_type = Source;

        /** The iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** The match id type. */
        using match_id_type = MatchId;

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

        /** This class' type. */
        using parse_context_type = parse_context<source_type, match_id_type, text_position_type, symbol_comparator_type>;

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
             * Returns the match count.
             * @return the match count.
             */
            size_t match_count() const {
                return m_match_count;
            }

        private:
            parse_position_type m_parse_position;
            size_t m_match_count;
            bool m_terminal_parsing_allowed;
            state(const parse_position_type& parse_pos, size_t match_count = 0, bool tpa = false)
                : m_parse_position(parse_pos)
                , m_match_count(match_count)
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
         * Returns the current parse position.
         * @return the current parse position.
         */
        const parse_position_type& parse_position() const {
            return m_parse_position;
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
            m_parse_position.increment();
        }

        /**
         * Increments the parse position (iterator and text position column) by a specific count.
         * @param count number of columns to add to the current parse position.
         */
        void increment_parse_position(size_t count) {
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
         * @param id match id.
         * @param start_state start state.
         */
        void add_match(const match_id_type& id, const state& start_state) {
            match_container_type children(std::next(m_matches.begin(), start_state.m_match_count), m_matches.end());
            m_matches.resize(start_state.m_match_count);
            m_matches.push_back(match_type(id, start_state.m_parse_position, parse_position().iterator(), std::move(children)));
        }

        /**
         * Returns the current state of the parse context.
         * The state can be used to rewind the parse context to a prior state.
         * @return the current state of the parse context.
         */
        state get_state() const {
            return state(m_parse_position, m_matches.size(), m_terminal_parsing_allowed);
        }

        /**
         * Sets the current state of the parse context.
         * @param st the state to set.
         */
        void set_state(const state& st) {
            m_parse_position = st.m_parse_position;
            m_matches.resize(st.m_match_count);
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
            return m_symbol_comparator(a, b);
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
         * Parses an annotation.
         * The default implementation only invokes the given parse node for parsing.
         * @param parse_node the parse node to invoke for parsing.
         * @param annotation the annotation object.
         * @return true on success, false on failure.
         */
        template <class ParseNode, class Annotation>
        bool parse_annotation(const ParseNode& parse_node, const Annotation& annotation) {
            return parse_node.parse(*this);
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

        friend rule_type;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
