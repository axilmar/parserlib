#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <map>
#include <cctype>
#include <type_traits>
#include <string_view>


namespace parserlib {


    template <class ParseContext> class rule;


    /**
     * A comparator that returns the difference between two tokens as they are, without any conversion.
     */
    class case_sensitive_comparator {
    public:
        /**
         * The operator that computes the difference between two tokens.
         * @param t1 the first token.
         * @param t2 the second token.
         * @return the result of the operation t1 - t2.
         */
        template <class T1, class T2>
        int operator ()(const T1& t1, const T2& t2) const noexcept {
            return t1 - t2;
        }
    };


    /**
     * A comparator that returns the difference between two tokens after using the function std::tolower()
     * to convert them to lower case.
     */
    class case_insensitive_comparator {
    public:
        /**
         * The operator that computes the difference between two tokens.
         * @param t1 the first token.
         * @param t2 the second token.
         * @return the result of the operation std::tolower(t1) - std::tolower(t2).
         */
        template <class T1, class T2>
        int operator ()(const T1& t1, const T2& t2) const noexcept {
            return std::tolower(t1) - std::tolower(t2);
        }
    };


    /**
     * Helper function for creating a container of tokens from a range of tokens.
     * @param begin start of range.
     * @param end end of range.
     * @return either an std::string_view, if the element type conforms to an std::string's element requirements,
     *  or an std::vector.
     */
    template <class It>
    auto source(const It& begin, const It& end) {
        assert(begin <= end);
        using value_type = typename It::value_type;
        if constexpr (!std::is_array_v<value_type> && std::is_trivial_v<value_type> && std::is_standard_layout_v<value_type>) {
            return std::string_view(&*begin, std::distance(begin, end));
        }
        else {
            return std::vector(begin, end);
        }
    }


    /**
     * A parse error.
     * @param ErrorId id of error.
     * @param Iterator iterator for the input range.
     */
    template <class ErrorId, class Iterator>
    class parse_error {
    public:
        /** This class type alias. */
        using error_type = parse_error<ErrorId, Iterator>;

        /** Error id type. */
        using error_id_type = ErrorId;

        /** Iterator type. */
        using iterator_type = Iterator;

        /**
         * The constructor.
         * @param id the error id.
         * @param begin start of input range that represents the error.
         * @param end end of input range that represents the error.
         */
        parse_error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) noexcept
            : m_id(id)
            , m_begin(begin)
            , m_end(end)
        {
            assert(m_begin < m_end);
        }

        /**
         * Returns the error id.
         * @return the error id.
         */
        const error_id_type& id() const noexcept {
            return m_id;
        }

        /**
         * Returns the start of the erroneous input range.
         * @return the start of the erroneous input range.
         */
        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        /**
         * Returns the end of the erroneous input range.
         * @return the end of the erroneous input range.
         */
        const iterator_type& end() const noexcept {
            return m_end;
        }

        /**
         * Same as parserlib::source(begin(), end()).
         * @return a container with the source tokens.
         */
        auto source() const noexcept {
            return parserlib::source(m_begin, m_end);
        }

        /**
         * Compares two errors by begin position.
         * @param error1 first error.
         * @param error2 second error.
         * @return true if error1.begin() < error2.begin(), false otherwise.
         */
        friend bool operator < (const error_type& error1, const error_type& error2) noexcept {
            return error1.m_begin < error2.m_begin;
        }

    private:
        error_id_type m_id;
        iterator_type m_begin;
        iterator_type m_end;
    };


    /**
     * Base class for parse context extensions.
     */
    class parse_context_extension_base {
    };


    /**
     * An empty parse context extension.
     * It is the default extension of a parse context.
     */
    class empty_parse_context_extension : public parse_context_extension_base {
    };


    /**
     * Keeps the state required for parsing.
     * @param Source source type; must have STL container types.
     * @param MatchId id of match.
     * @param ErrorId id of error.
     * @param Comparator token comparator.
     * @param Extension parse context extension.
     */
    template <class Source, class MatchId = int, class ErrorId = int, class Comparator = case_sensitive_comparator, class Extension = empty_parse_context_extension>
    class parse_context {
    public:
        // TYPES --------------------------------------------------------------

        /** This parse context type. */
        using parse_context_type = parse_context<Source, MatchId, ErrorId, Comparator>;

        /** The source type. */
        using source_type = Source;

        /** The token type. */
        using value_type = typename Source::value_type;

        /** The match id type. */
        using match_id_type = MatchId;

        /** The error id type. */
        using error_id_type = ErrorId;

        /** The comparator type. */
        using comparator_type = Comparator;

        /** The iterator type. */
        using iterator_type = typename source_type::const_iterator;

        /** The rule type for this parse context. */
        using rule_type = rule<parse_context_type>;

        /** The extension type. */
        using extension_type = Extension;

        // CONSTRUCTOR --------------------------------------------------------

        /**
         * The constructor.
         * @param source the source to parse.
         * @param extension the extension.
         */
        parse_context(Source& source, const Extension& extension = Extension()) noexcept
            : m_source(source)
            , m_parse_position(source.begin())
            , m_extension(extension)
        {
        }

        // STATE --------------------------------------------------------------

        /**
         * Represents an instance of the parse context state.
         * It can be used to rewind the parse context to a specific state.
         */
        class state {
        public:
            /**
             * The constructor.
             * @param position current parse position.
             * @param match_count number of matches in the internal match stack.
             */
            state(const iterator_type& position, const std::size_t match_count) noexcept
                : m_position(position)
                , m_match_count(match_count)
            {
            }

            /**
             * Returns the parse position.
             * @return the parse position.
             */
            const iterator_type& position() const noexcept {
                return m_position;
            }

            /**
             * Returns the match count.
             * @return the match count.
             */
            std::size_t match_count() const noexcept {
                return m_match_count;
            }

        private:
            iterator_type m_position;
            std::size_t m_match_count;
        };

        /** The state type. */
        using state_type = class state;

        /**
         * Returns the current source.
         * @return the current source.
         */
        const Source& source() const noexcept {
            return m_source;
        }

        /**
         * Returns the current parse position.
         * @return the current parse position.
         */
        const iterator_type& parse_position() const noexcept {
            return m_parse_position;
        }

        /**
         * Returns the end parse position.
         * Equal to source().end().
         * @return the end parse position.
         */
        auto end_parse_position() const noexcept {
            return m_source.end();
        }

        /**
         * Returns true if the current parse position is less than the end position.
         * @return parse_position() < end_position().
         */
        bool is_valid_parse_position() const noexcept {
            return m_parse_position < m_source.end();
        }

        /**
         * Returns true if the current parse position is equal to the end position.
         * @return parse_position() == end_position().
         */
        bool is_end_parse_position() const noexcept {
            return m_parse_position == m_source.end();
        }

        /**
         * Sets the parse position.
         * @param position the new parse position; if invalid, then the parse context gets to an undefined state.
         */
        void set_parse_position(const iterator_type& position) noexcept {
            assert(position <= m_source.end());
            m_parse_position = position;
        }

        /**
         * Increments the parse position.
         * No check is performed in release mode, for performance reasons;
         * this means that if the current parse position points to the end of the input,
         * then incrementing it is an undefined operation.
         */
        void increment_parse_position() noexcept {
            assert(m_parse_position < m_source.end());
            ++m_parse_position;
        }

        /**
         * Increments the parse position by a specific number of positions.
         * No check is performed in release mode, for performance reasons;
         * this means that if the current parse position points to the end of the input,
         * then incrementing it is an undefined operation.
         * @param connt number of positions to add to the current position.
         */
        void increment_parse_position(std::size_t count) noexcept {
            assert(std::next(m_parse_position, count) <= m_source.end());
            m_parse_position = std::next(m_parse_position, count);
        }

        /**
         * Records and returns the current state.
         * @return the current state.
         */
        class state state() const noexcept {
            return { m_parse_position, m_matches.size() };
        }

        /**
         * Sets the current state.
         * No check is performed for the state's validity, for performance reasons.
         * @param state state to set.
         */
        void set_state(const class state& state) noexcept {
            assert(state.position() <= m_source.end());
            m_parse_position = state.position();
            m_matches.resize(state.match_count());
        }

        // COMPARISONS --------------------------------------------------------

        /**
         * Compares two tokens by instantiating and invoking the comparator type
         * passed as a template argument.
         * @param t1 first token.
         * @param t2 second token.
         * @return whatever comparator_type::operator ()(t1, t2) returns.
         */
        template <class T1, class T2>
        static int compare(const T1& t1, const T2& t2) noexcept {
            return comparator_type()(t1, t2);
        }

        // MATCHES ------------------------------------------------------------

        /**
         * A match.
         */
        class match {
        public:
            /**
             * The default constructor.
             */
            match() {
            }

            /**
             * Constructor from parameters.
             * @param id id of match.
             * @param begin start of the part of the input that corresponds to this match.
             * @param end end of the part of the input that corresponds to this match.
             * @param children children matches.
             */
            match(const match_id_type& id, const iterator_type& begin, const iterator_type& end, std::vector<match>&& children) noexcept
                : m_id(id)
                , m_begin(begin)
                , m_end(end)
                , m_children(std::move(children))
            {
                assert(m_begin < m_end);
            }

            /**
             * Returns the id of the match.
             * @return the id of the match.
             */
            const match_id_type& id() const noexcept {
                return m_id;
            }

            /**
             * Returns the start of the match in the input.
             * @return the start of the match in the input.
             */
            const iterator_type& begin() const noexcept {
                return m_begin;
            }

            /**
             * Returns the end of the match in the input.
             * @return the end of the match in the input.
             */
            const iterator_type& end() const noexcept {
                return m_end;
            }

            /**
             * Returns the children matches.
             * @return the children matches.
             */
            const std::vector<match>& children() const noexcept {
                return m_children;
            }

            /**
             * Returns the source that corresponds to the input range that was matched.
             * Same as parserlib::source(begin(), end()).
             * @return a container with the tokens that corresponds to the input range that was matched.
             */
            auto source() const noexcept {
                return parserlib::source(m_begin, m_end);
            }

        private:
            match_id_type m_id;
            iterator_type m_begin;
            iterator_type m_end;
            std::vector<match> m_children;
        };

        /** Container of matches. */
        using match_container_type = std::vector<match>;

        /**
         * Returns the current matches.
         * @return the current matches.
         */
        const match_container_type& matches() const noexcept {
            return m_matches;
        }

        /**
         * Adds a match to this parse context.
         * @param id match id.
         * @param begin start of matched input.
         * @param end end of matched input.
         * @param match_count number of matches recorded before the match was parsed.
         *  The matches from match_count to end are added as children to the new match.
         */
        void add_match(const match_id_type& id, const iterator_type& begin, const iterator_type& end, const std::size_t match_count) noexcept {
            assert(match_count <= m_matches.size());
            const auto matches_begin = std::next(m_matches.begin(), match_count);
            const auto matches_end = m_matches.end();
            match new_match{ id, begin, end, { matches_begin, matches_end } };
            m_matches.resize(match_count);
            m_matches.push_back(std::move(new_match));
        }

        /**
         * Adds a match without children.
         * @param id match id.
         * @param begin start of matched input.
         * @param end end of matched input.
         */
        void add_match(const match_id_type& id, const iterator_type& begin, const iterator_type& end) noexcept {
            add_match(id, begin, end, m_matches.size());
        }

        // ERRORS -------------------------------------------------------------

        /** The error type. */
        using error_type = parse_error<error_id_type, iterator_type>;

        /** The error container type. */
        using error_container_type = std::vector<error_type>;

        /**
         * Returns the errors.
         * @return the errors.
         */
        const error_container_type& errors() const noexcept {
            return m_errors;
        }

        /**
         * Adds an error.
         * @param id id of error.
         * @param begin start of erroneous input.
         * @param end end of erroneous input.
         */
        void add_error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) noexcept {
            m_errors.emplace_back(id, begin, end);
        }

        /**
         * Sorts the errors based on their start position.
         */
        void sort_errors() {
            std::sort(m_errors.begin(), m_errors.end());
        }

        // RULES ------------------------------------------------------------------

        /**
         * Checks if the given rule is left-recursive in the current parse position.
         * @param rule rule to check for.
         * @return true if the given rule is left-recursive in the current position, false otherwise.
         */
        bool is_left_recursive_rule(rule_type& rule) const noexcept {
            const auto it = m_rule_parse_positions.find(rule.pointer());
            return it != m_rule_parse_positions.end() && !it->second.empty() ? it->second.back() == m_parse_position : false;
        }

        /**
         * Records the current parse position for the given rule, allowing the rule to know if it is left-recursive or not.
         * @param rule the rule to record the parse position for.
         */
        void push_rule_parse_position(rule_type& rule) noexcept {
            m_rule_parse_positions[rule.pointer()].push_back(m_parse_position);
        }

        /**
         * Pops the last-recorded parse position for the given rule from the internal per-rule stack.
         * @param rule the rule to remove the last-recorded parse position for.
         */
        void pop_rule_parse_position(rule_type& rule) noexcept {
            assert(!m_rule_parse_positions[rule.pointer()].empty());
            m_rule_parse_positions[rule.pointer()].pop_back();
        }

        // EXTENSION ----------------------------------------------------------

        /**
         * Returns the current extension (const version).
         * @return the current extension.
         */
        const extension_type& extension() const noexcept {
            return m_extension;
        }

        /**
         * Returns the current extension (mutable version).
         * @return the current extension.
         */
        extension_type& extension() noexcept {
            return m_extension;
        }

    private:
        using rule_parse_position_map_type = std::map<rule_type*, std::vector<iterator_type>>;

        Source& m_source;
        iterator_type m_parse_position;
        match_container_type m_matches;
        error_container_type m_errors;
        rule_parse_position_map_type m_rule_parse_positions;
        extension_type m_extension;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
