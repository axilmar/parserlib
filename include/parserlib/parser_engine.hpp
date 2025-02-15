#ifndef PARSERLIB_PARSER_ENGINE_HPP
#define PARSERLIB_PARSER_ENGINE_HPP


#include <string>
#include <set>
#include <utility>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <string_view>
#include <functional>
#include <typeinfo>
#include "compare.hpp"


namespace parserlib {


    /**
     * A class that defines the algorithms and data structures for parsing.
     *
     * More specifically, it provides:
     *
     * - typedefs for common types.
     * - the parse context class used for parsing.
     * - the EBNF grammar implementation classes.
     * - the EBNF operators.
     * - functionality for converting the matches to an Abstract Syntax Tree.
     *
     * @param SourceT type of source; must be an STL-compatible container.
     * @param MatchIdT type of id for matches; it defaults to 'int', if a special enumeration is not provided.
     */
    template <typename SourceT = std::string, typename MatchIdT = int> class parser_engine {
    public:
        class parse_context;
        class match;
        class rule_reference_parser;
        class rule;
        template <typename ParserT, typename ContFuncT> class error_parser;
        template <typename ParserT> class zero_or_more_parser;
        template <typename ParserT> class one_or_more_parser;
        template <typename ParserT> class optional_parser;
        template <typename ParserT> class logical_and_parser;
        template <typename ParserT> class logical_not_parser;
        template <typename ParserT> class match_parser;
        template <typename ParserT, typename MatchFuncT> class custom_match_parser;

        /**
         * Type of this class.
         */
        using class_type = parser_engine<SourceT, MatchIdT>;

        /**
         * Type of source.
         */
        using source_type = SourceT;

        /**
         * Type of terminal value; defined by the value type of source.
         */
        using terminal_value_type = typename SourceT::value_type;

        /**
         * Type of basic string for the terminal value.
         */
        using terminal_string_type = std::basic_string<terminal_value_type>;

        /**
         * Type of iterator over the source.
         */
        using iterator_type = typename source_type::const_iterator;

        /**
         * Type of match id.
         */
        using match_id_type = MatchIdT;

        /**
         * Type of match container.
         */
        using match_container_type = std::vector<match>;

        /**
         * A match.
         *
         * It represents a portion of the source that is matched by a grammar.
         */
        class match {
        public:
            /**
             * The default constructor.
             */
            match()
                : m_id()
            {
            }

            /**
             * Constructor from parameters.
             * @param match_id id of match.
             * @param start_position start position of match, into the source.
             * @param end_position end position of match, into the source (one position after the last).
             * @param children children matches.
             */
            match(match_id_type match_id, const iterator_type& start_position, const iterator_type& end_position, match_container_type&& children = {})
                : m_id(match_id)
                , m_start_position(start_position)
                , m_end_position(end_position)
                , m_children(std::move(children))
            {
            }

            /**
             * Returns the id of the match.
             * @return the id of the match.
             */
            match_id_type get_id() const {
                return m_id;
            }

            /**
             * Returns the start position of the match.
             * @return the start position of the match.
             */
            const iterator_type& get_start_position() const {
                return m_start_position;
            }

            /**
             * Returns the end position of the match.
             * @return the end position of the match.
             */
            const iterator_type& get_end_position() const {
                return m_end_position;
            }

            /**
             * Returns the children matches.
             * @return the children matches.
             */
            const match_container_type& get_children() const {
                return m_children;
            }

            /**
             * Returns the child at the specified index.
             * No bounds checking is performed.
             * @return the child match at the specified index.
             */
            const match& operator [](size_t index) const {
                return m_children[index];
            }

            /**
             * Locates a child match with the given id.
             * @param id id of child match.
             * @return the match found or null if none is found.
             */
            const match* find_child_by_id(match_id_type id) const {
                for (const match& child_match : m_children) {
                    if (child_match.get_id() == id) {
                        return &child_match;
                    }
                }
                return nullptr;
            }

            /**
             * Returns a view of the source that corresponds to this match.
             * @return a view of the source that corresponds to this match.
             */
            auto get_source() const {
                return get_source_helper(m_start_position, m_end_position);
            }

        private:
            match_id_type m_id{};
            iterator_type m_start_position{};
            iterator_type m_end_position{};
            size_t m_child_count{};
            match_container_type m_children{};
        };

        /**
         * Result of parsing.
         */
        enum class parse_result : bool {
            /**
             * Failure equals false.
             */
            failure = false,

            /**
             * Success equals true.
             */
            success = true
        };

        /**
         * Error information.
         */
        class error_info {
        public:
            /**
             * The constructor.
             * @param id error id.
             * @param position error position.
             */
            error_info(int id = 0, const iterator_type& position = iterator_type())
                : m_id(id)
                , m_position(position)
            {
            }

            /**
             * Returns the id.
             * @return the id.
             */
            int get_id() const {
                return m_id;
            }

            /**
             * Returns the error position.
             * @return the error position.
             */
            const iterator_type& get_position() const {
                return m_position;
            }

        private:
            int m_id;
            iterator_type m_position;
        };

        /**
         * Type of error container.
         */
        using error_container_type = std::vector<error_info>;

        /**
         * A parse context.
         *
         * It keeps the state of parsing.
         */
        class parse_context {
        public:
            /**
             * State of the parse context.
             * It can be used to restore the parse contet to a particular state.
             */
            class state {
            public:

            private:
                iterator_type m_position;
                size_t m_matches_size;
                size_t m_errors_size;

                state(const iterator_type& si, size_t matches_size, size_t errors_size)
                    : m_position(si)
                    , m_matches_size(matches_size)
                    , m_errors_size(errors_size)
                {
                }

                friend class parse_context;
            };

            /**
             * Constructor.
             * @param input the source; must be in scope while this parse context is in scope, since the matches produced
             *  create views on the source.
             *  is empty upon return.
             */
            parse_context(source_type& input)
                : m_current_position(input.begin())
                , m_end_position(input.end())
                , m_unparsed_position(input.begin())
            {
            }

            /**
             * Constructor.
             * @param begin beginning of source.
             * @param end end of source.
             */
            parse_context(const iterator_type& begin, const iterator_type& end)
                : m_current_position(begin)
                , m_end_position(end)
                , m_unparsed_position(begin)
            {
            }

            /**
             * Returns the current parsing position.
             * @return the current parsing position.
             */
            const iterator_type& get_current_position() const {
                return m_current_position;
            }

            /**
             * Returns the end position of the input.
             * @return the end position of the input.
             */
            const iterator_type& get_end_position() const {
                return m_end_position;
            }

            /**
             * Checks if the end position is reached.
             * @return false if the end position is reached, true otherwise.
             */
            bool is_valid_position() const {
                return m_current_position != m_end_position;
            }

            /**
             * Checks if the end position is reached.
             * @return true if the end position is reached, false otherwise.
             */
            bool is_end_position() const {
                return m_current_position == m_end_position;
            }

            /**
             * Increments the current position.
             * No checks are performed, for performance reasons.
             */
            void increment_position() {
                ++m_current_position;
                if (m_current_position > m_unparsed_position) {
                    m_unparsed_position = m_current_position;
                }
            }

            /**
             * Increments the current position by the given amount.
             * @param count number of characters to advance the iterator.
             * No checks are performed, for performance reasons.
             */
            void increment_position(size_t count) {
                m_current_position += count;
                if (m_current_position > m_unparsed_position) {
                    m_unparsed_position = m_current_position;
                }
            }

            /**
             * Returns the current state of the parse context.
             * @return the current state of the parse context.
             */
            state get_state() const {
                return { m_current_position, m_matches.size(), m_errors.size() };
            }

            /**
             * Sets the state of the parse context.
             * @param st the state to set.
             */
            void restore_state(const state& st) {
                m_current_position = st.m_position;
                m_matches.resize(st.m_matches_size);
                m_errors.resize(st.m_errors_size);
            }

            /**
             * Returns the container of the matches.
             * @return the container of the matches.
             */
            const match_container_type& get_matches() const {
                return m_matches;
            }

            /**
             * Returns the error container.
             * @return the error container.
             */
            const error_container_type& get_errors() const {
                return m_errors;
            }

            /**
             * Adds an error to this parse context.
             * @param id error id.
             * @param position error position.
             */
            void add_error(int id, const iterator_type& position) {
                m_errors.emplace_back(id, position);
            }

            /**
             * Returns the furthest position that is not yet parsed.
             * @return the furthest position that is not yet parsed.
             */
            const iterator_type& get_unparsed_position() const {
                return m_unparsed_position;
            }

        private:
            struct left_recursion_match_position {
                iterator_type start_position;
                size_t start_size;
                iterator_type accept_position;
            };

            iterator_type m_current_position;
            iterator_type m_end_position;
            match_container_type m_matches;
            std::map<rule*, std::vector<iterator_type>> m_rule_parse_positions;
            std::vector<left_recursion_match_position> m_left_recursion_match_positions;
            error_container_type m_errors;
            iterator_type m_unparsed_position;

            void add_match(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, size_t child_count) {
                if (child_count > m_matches.size()) {
                    throw std::logic_error("parser_engine: parse_context: add_match: invalid child count.");
                }
                auto start = std::prev(m_matches.end(), child_count);
                auto end = m_matches.end();
                match_container_type child_matches(start, end);
                m_matches.erase(start, end);
                m_matches.emplace_back(id, start_position, end_position, std::move(child_matches));
            }

            void add_match(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, size_t child_count, match_container_type&& child_matches) {
                if (child_count > m_matches.size()) {
                    throw std::logic_error("parser_engine: parse_context: add_match: invalid child count.");
                }
                auto start = std::prev(m_matches.end(), child_count);
                auto end = m_matches.end();
                m_matches.erase(start, end);
                m_matches.emplace_back(id, start_position, end_position, std::move(child_matches));
            }

            std::tuple<iterator_type, size_t> get_match_start() const {
                if (!m_left_recursion_match_positions.empty()) {
                    const left_recursion_match_position& pos = m_left_recursion_match_positions.back();
                    if (m_current_position == pos.accept_position) {
                        return std::make_tuple(pos.start_position, pos.start_size);
                    }
                }
                return std::make_tuple(m_current_position, m_matches.size());
            }

            std::tuple<iterator_type, size_t> get_match_end() const {
                return std::make_tuple(m_current_position, m_matches.size());
            }

            bool is_left_recursive_parse_position(rule* r) const {
                auto it = m_rule_parse_positions.find(r);
                return it != m_rule_parse_positions.end() && !it->second.empty() && it->second.back() == m_current_position;
            }

            void push_parse_position(rule* r) {
                m_rule_parse_positions[r].push_back(m_current_position);
            }

            void pop_parse_position(rule* r) {
                m_rule_parse_positions[r].pop_back();
            }

            void push_left_recursion_match_position(const iterator_type& start_position, size_t start_size, const iterator_type& accept_position) {
                m_left_recursion_match_positions.push_back(left_recursion_match_position{ start_position, start_size, accept_position });
            }

            void pop_left_recursion_match_position() {
                m_left_recursion_match_positions.pop_back();
            }

            parse_result parse_rule(rule& r) {
                return r.parse(*this);
            }

            parse_result parse_rule_left_recursion_base(rule& r) {
                return r.parse_left_recursion_base(*this);
            }

            parse_result parse_rule_left_recursion_continuation(rule& r) {
                return r.parse_left_recursion_continuation(*this);
            }

            void set_unparsed_position(const iterator_type& position) {
                m_unparsed_position = position;
            }

            template <typename ParserT, typename ContFuncT> friend class error_parser;
            template <typename ParserT> friend class match_parser;
            template <typename ParserT, typename MatchFuncT> friend class custom_match_parser;
            friend class rule_reference_parser;
            friend class rule;
        };

        /**
         * Base class for grammar implementation classes.
         *
         * It allows derived classes to get operators for zero or more, one or more, optional, logical and, and logical or parsers.
         *
         * @param DerivedT type of derived class.
         */
        template <typename DerivedT>
        class parser {
        public:
            /**
             * Converts this parser to a zero-or-more parser.
             * @return a zero-or-more parser for this parser.
             */
            zero_or_more_parser<DerivedT> operator *() const {
                return { *static_cast<const DerivedT*>(this) };
            }

            /**
             * Converts this parser to a one-or-more parser.
             * @return a one-or-more parser for this parser.
             */
            one_or_more_parser<DerivedT> operator +() const {
                return { *static_cast<const DerivedT*>(this) };
            }

            /**
             * Converts this parser to an optional parser.
             * @return an optional parser for this parser.
             */
            optional_parser<DerivedT> operator -() const {
                return { *static_cast<const DerivedT*>(this) };
            }

            /**
             * Converts this parser to a logical-and parser.
             * @return a logical-and parser for this parser.
             */
            logical_and_parser<DerivedT> operator &() const {
                return { *static_cast<const DerivedT*>(this) };
            }

            /**
             * Converts this parser to a logical-or parser.
             * @return a logical-or parser for this parser.
             */
            logical_not_parser<DerivedT> operator !() const {
                return { *static_cast<const DerivedT*>(this) };
            }
        };

        /**
         * A parser for terminal values.
         */
        template <typename ValueT>
        class terminal_value_parser : public parser<terminal_value_parser<ValueT>> {
        public:
            /**
             * Constructor.
             * @param value the terminal value.
             */
            terminal_value_parser(const ValueT& value)
                : m_value(value)
            {
            }

            /**
             * Constructor.
             * @param value the terminal value.
             */
            terminal_value_parser(ValueT&& value)
                : m_value(std::move(value))
            {
            }

            /**
             * Parses the given input against a single value.
             *
             * It checks if the character at the current position equals the value stored internally.
             * If true, it increments the parse position and returns true.
             *
             * Otherwise it returns false.
             *
             * @param pc current parse context.
             *
             * @return success if parsing suceeds, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                if (pc.is_valid_position() && compare_equal(*pc.get_current_position(), m_value)) {
                    pc.increment_position();
                    return parse_result::success;
                }
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            ValueT m_value;
        };

        /**
         * A parser that uses a function to test if a token is acceptable.
         * @param F type of function to store internally.
         */
        template <typename F>
        class terminal_function_parser : public parser<terminal_function_parser<F>> {
        public:
            /**
             * function type.
             */
            using function_type = F;

            /**
             * Constructor.
             * @param f function to use for parsing.
             */
            terminal_function_parser(const function_type& f) : m_function(f) {
            }

            /**
             * Constructor.
             * @param f function to use for parsing; moved object.
             */
            terminal_function_parser(function_type&& f) : m_function(std::move(f)) {
            }

            /**
             * Uses the internal function to test if the current token is acceptable.
             * The function must return non-zero if the input token is accepted.
             * @param pc parse context.
             * @return success if the function returned non-zero, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                if (pc.is_valid_position() && m_function(*pc.get_current_position())) {
                    pc.increment_position();
                    return parse_result::success;
                }
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            function_type m_function;
        };

        /**
         * A parser for strings.
         */
        template <typename StringT>
        class terminal_string_parser : public parser<terminal_string_parser<StringT>> {
        public:
            /**
             * Constructor from null-terminated string.
             * @param string string to parse.
             */
            terminal_string_parser(const StringT& string)
                : m_string(string)
            {
            }

            /**
             * Constructor from null-terminated string.
             * @param string string to parse; moved object.
             */
            terminal_string_parser(StringT&& string)
                : m_string(std::move(string))
            {
            }

            /**
             * Parses the given input against a string of values.
             *
             * It checks if the string at the current position equals the string stored internally.
             * If true, it increments the parse position by the string size and returns true.
             *
             * Otherwise it returns false.
             *
             * @param pc current parse context.
             *
             * @return success if parsing suceeds, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto it_str = m_string.begin();
                auto it_pc = pc.get_current_position();
                for (;;) {
                    if (it_str == m_string.end()) {
                        break;
                    }
                    if (it_pc == pc.get_end_position()) {
                        return parse_result::failure;
                    }
                    if (!compare_equal(*it_pc, *it_str)) {
                        return parse_result::failure;
                    }
                    ++it_str;
                    ++it_pc;
                }

                pc.increment_position(m_string.size());
                return parse_result::success;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            StringT m_string;
        };

        /**
         * A parser for a set of terminal values.
         *
         * Internally, it uses a sorted flat array, and std::upper_bound to locate the appropriate set member.
         */
        template <typename ValueT>
        class terminal_set_parser : public parser<terminal_set_parser<ValueT>> {
        public:
            /**
             * Constructor from terminal value types.
             * @param values values of the set.
             */
            terminal_set_parser(std::initializer_list<ValueT> values)
                : m_set(values)
            {
                std::sort(m_set.begin(), m_set.end());
            }

            /**
             * Constructor from string of values.
             * @param values values of the set.
             */
            terminal_set_parser(const std::basic_string<ValueT>& values)
                : m_set(values.begin(), values.end())
            {
                std::sort(m_set.begin(), m_set.end());
            }

            /**
             * Constructor from null-terminated string of values.
             * @param values values of the set.
             */
            terminal_set_parser(const ValueT* values)
                : terminal_set_parser(std::basic_string<ValueT>(values))
            {
            }

            /**
             * Parses the given input against a set of values.
             *
             * It checks if the string at the current position equals one the values stored internally.
             * If true, it increments the parse position and returns true.
             *
             * Otherwise it returns false.
             *
             * @param pc current parse context.
             *
             * @return success if parsing suceeds, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                if (pc.is_valid_position()) {
                    const auto val = *pc.get_current_position();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), val, parserlib::less_than());
                    if (it != m_set.begin()) {
                        const auto pv = *std::prev(it);
                        if (pv == val) {
                            pc.increment_position();
                            return parse_result::success;
                        }
                    }
                }
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            std::vector<ValueT> m_set;
        };

        /**
         * A parser for a range of terminal values.
         */
        template <typename ValueT>
        class terminal_range_parser : public parser<terminal_range_parser<ValueT>> {
        public:
            /**
             * Constructor.
             * @param min min value.
             * @param max max value.
             * @exception std::logic_error thrown if min is greater than max.
             */
            terminal_range_parser(const ValueT& min, const ValueT& max)
                : m_min(min)
                , m_max(max)
            {
                if (min > max) {
                    throw std::logic_error("terminal_range: invalid range.");
                }
            }

            /**
             * Constructor.
             * @param min min value.
             * @param max max value.
             * @exception std::logic_error thrown if min is greater than max.
             */
            terminal_range_parser(ValueT&& min, ValueT&& max)
                : m_min(std::move(min))
                , m_max(std::move(max))
            {
                if (min > max) {
                    throw std::logic_error("terminal_range: invalid range.");
                }
            }

            /**
             * Parses the given input against a range of values.
             *
             * It checks if the string at the current position is within the range of values stored internally.
             * If true, it increments the parse position and returns true.
             *
             * Otherwise it returns false.
             *
             * @param pc current parse context.
             *
             * @return success if parsing suceeds, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                if (pc.is_valid_position()) {
                    const auto val = *pc.get_current_position();
                    if (compare_greater_than_or_equal_to(val, m_min) && compare_less_than_or_equal_to(val, m_max)) {
                        pc.increment_position();
                        return parse_result::success;
                    }
                }
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            ValueT m_min;
            ValueT m_max;
        };

        /**
         * A parser object that allows any function to become a parser.
         * @param FuncT function type; it must have the signature parse_result (parse_context&).
         */
        template <typename FuncT>
        class function_parser : public parser<function_parser<FuncT>> {
        public:
            /**
             * Constructor.
             * @param func function.
             */
            function_parser(const FuncT& func)
                : m_func(func)
            {
            }

            /**
             * Constructor.
             * @param func function; moved object.
             */
            function_parser(FuncT&& func)
                : m_func(std::move(func))
            {
            }

            /**
             * Invokes the internal function to parse, if the parsing position is valid.
             * @param pc parse context.
             * @return what the internal function returns, or failure if the input end position has been reached.
             */
            parse_result parse(parse_context& pc) const {
                if (pc.is_valid_position()) {
                    return m_func(pc);
                }
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_result::failure;
            }

        private:
            FuncT m_func;
        };

        /**
         * An error parser.
         * @param ParserT type of grammar to use for error parsing.
         * @param ContFuncT continuation function.
         */
        template <typename ParserT, typename ContFuncT>
        class error_parser : public parser<error_parser<ParserT, ContFuncT>> {
        public:
            /**
             * Constructor.
             * @param parser grammar.
             * @param error_id error id.
             * @paam cont_func continuation function.
             */
            error_parser(const ParserT& parser, int error_id, ContFuncT&& cont_func)
                : m_parser(parser)
                , m_error_id(error_id)
                , m_cont_func(std::move(cont_func))
            {
            }

            /**
             * Constructor.
             * @param parser grammar; moved object.
             * @param error_id error id.
             * @paam cont_func continuation function.
             */
            error_parser(ParserT&& parser, int error_id, ContFuncT&& cont_func)
                : m_parser(std::move(parser))
                , m_error_id(error_id)
                , m_cont_func(std::move(cont_func))
            {
            }

            /**
             * Invokes the internal parser; if the internal parser returns error,
             * then this class adds an error to the parse context, then tries to continue parsing
             * using the supplied continuation function.
             * @param pc parse context.
             * @return if success, then either the internal parser did not create an error
             *  or the continuation function managed to sety up the parse context for continuing the parsing;
             *  otherwise failure.
             */
            parse_result parse(parse_context& pc) const {
                iterator_type prev_unparsed_position = pc.get_unparsed_position();
                pc.set_unparsed_position(pc.get_current_position());
                try {
                    if (m_parser.parse(pc) == parse_result::success) {
                        return parse_result::success;
                    }
                }
                catch (...) {
                    pc.set_unparsed_position(prev_unparsed_position);
                    throw;
                }
                pc.add_error(m_error_id, pc.get_unparsed_position());
                pc.set_unparsed_position(prev_unparsed_position);
                return m_cont_func(pc);
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                iterator_type prev_unparsed_position = pc.get_unparsed_position();
                pc.set_unparsed_position(pc.get_current_position());
                try {
                    if (m_parser.parse_left_recursion_base(pc) == parse_result::success) {
                        return parse_result::success;
                    }
                }
                catch (...) {
                    pc.set_unparsed_position(prev_unparsed_position);
                    throw;
                }
                pc.add_error(m_error_id, pc.get_unparsed_position());
                pc.set_unparsed_position(prev_unparsed_position);
                return m_cont_func(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                iterator_type prev_unparsed_position = pc.get_unparsed_position();
                pc.set_unparsed_position(pc.get_current_position());
                try {
                    if (m_parser.parse_left_recursion_continuation(pc) == parse_result::success) {
                        return parse_result::success;
                    }
                }
                catch (...) {
                    pc.set_unparsed_position(prev_unparsed_position);
                    throw;
                }
                pc.add_error(m_error_id, pc.get_unparsed_position());
                pc.set_unparsed_position(prev_unparsed_position);
                return m_cont_func(pc);
            }

        private:
            ParserT m_parser;
            int m_error_id;
            ContFuncT m_cont_func;
        };

        /**
         * A parser that creates a loop out of another parser.
         * The loop is optional, i.e. it can parse zero or more times.
         * @param ParserT type of parser to create a loop of.
         */
        template <typename ParserT>
        class zero_or_more_parser : public parser<zero_or_more_parser<ParserT>> {
        public:
            /**
             * Constructor from copy of parser.
             * @param parser parser to create a loop out of.
             */
            zero_or_more_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor from parser; the object is moved.
             * @param parser parser to create a loop out of.
             */
            zero_or_more_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * Optimization for creating a loop out of a loop.
             * @return reference to this.
             */
            const zero_or_more_parser<ParserT>& operator *() const {
                return *this;
            }

            /**
             * Optimization for creating a one-or-more loop out of a zero-or-more loop.
             * @return a zero or more parser with the internal parser as a parser.
             */
            one_or_more_parser<ParserT> operator +() const {
                return m_parser;
            }

            /**
             * Optimization for creating an optional parser out of a zero-or-more loop.
             * @return reference to this; a zero-or-more loop is already optional.
             */
            const zero_or_more_parser<ParserT>& operator -() const {
                return *this;
            }

            /**
             * Parses the given input repeatedly against a parser, until the parser returns failure.
             *
             * @param pc current parse context.
             *
             * @return always success.
             */
            parse_result parse(parse_context& pc) const {
                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }
                return parse_result::success;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse_left_recursion_base(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }
                return parse_result::success;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse_left_recursion_continuation(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }
                return parse_result::success;
            }

        private:
            ParserT m_parser;
        };

        /**
         * A parser that creates a loop out of another parser.
         * The loop is should parse successfully at least once, i.e. it can parse one or more times.
         * @param ParserT type of parser to create a loop of.
         */
        template <typename ParserT>
        class one_or_more_parser : public parser<one_or_more_parser<ParserT>> {
        public:
            /**
             * Constructor from copy of parser.
             * @param parser parser to create a loop out of.
             */
            one_or_more_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor from parser; the object is moved.
             * @param parser parser to create a loop out of.
             */
            one_or_more_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * Optimization for creating a one-or-more loop out of this.
             * @return reference to this.
             */
            const one_or_more_parser<ParserT>& operator +() const {
                return *this;
            }

            /**
             * Optimization for creating a zero-or-more loop out of this.
             * @return a zero-or-more parser with the internal parser as its parser.
             */
            zero_or_more_parser<ParserT> operator *() const {
                return m_parser;
            }

            /**
             * Optimization for creating an optional loop out of this.
             * @return a zero-or-more parser with the internal parser as its parser.
             */
            zero_or_more_parser<ParserT> operator -() const {
                return m_parser;
            }

            /**
             * Parses the given input repeatedly against a parser, until the parser returns failure.
             *
             * @param pc current parse context.
             *
             * @return failure if parsing once fails, success otherwise.
             */
            parse_result parse(parse_context& pc) const {
                parse_result result = m_parser.parse(pc);
                if (result == parse_result::failure) {
                    return parse_result::failure;
                }

                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }

                return parse_result::success;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                parse_result result = m_parser.parse_left_recursion_base(pc);
                if (result == parse_result::failure) {
                    return parse_result::failure;
                }

                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse_left_recursion_base(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }

                return parse_result::success;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                parse_result result = m_parser.parse_left_recursion_continuation(pc);
                if (result == parse_result::failure) {
                    return parse_result::failure;
                }

                while (pc.is_valid_position()) {
                    parse_result result = m_parser.parse_left_recursion_continuation(pc);
                    if (result == parse_result::failure) {
                        break;
                    }
                }

                return parse_result::success;
            }

        private:
            ParserT m_parser;
        };

        /**
         * A parser that makes another parser optional.
         * @param ParserT type of parser to make optional.
         */
        template <typename ParserT>
        class optional_parser : public parser<optional_parser<ParserT>> {
        public:
            /**
             * Constructor from copy of parser.
             * @param parser parser to create a loop out of.
             */
            optional_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor from parser; the object is moved.
             * @param parser parser to create a loop out of.
             */
            optional_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * Optimization for creating an optional parser out of an optional parser.
             * @return reference to this.
             */
            const optional_parser<ParserT>& operator -() const {
                return *this;
            }

            /**
             * Invokes the internal parser, discarding the result.
             * @param pc parse context.
             * @return always success.
             */
            parse_result parse(parse_context& pc) const {
                m_parser.parse(pc);
                return parse_result::success;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                m_parser.parse_left_recursion_base(pc);
                return parse_result::success;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                m_parser.parse_left_recursion_continuation(pc);
                return parse_result::success;
            }

        private:
            ParserT m_parser;
        };

        /**
         * A parser that uses another parser as a logical and test.
         * No side effects are created from the parsing.
         * @param ParserT the type of parser to make a test out of.
         */
        template <typename ParserT>
        class logical_and_parser : public parser<logical_and_parser<ParserT>> {
        public:
            /**
             * Constructor from copy of parser.
             * @param parser parser to create a loop out of.
             */
            logical_and_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor from parser; the object is moved.
             * @param parser parser to create a loop out of.
             */
            logical_and_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * Optimization for creating a logical and parser out of a logical and parser.
             * @@return reference to this.
             */
            const logical_and_parser<ParserT>& operator &() const {
                return *this;
            }

            /**
             * Parses the input with the help of the internal parser,
             * then restores the parse context to the state that it was
             * before the parsing.
             * @param pc parse context.
             * @return success if the internal parser returns success, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse(pc);
                pc.restore_state(state);
                return result;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse_left_recursion_base(pc);
                pc.restore_state(state);
                return result;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse_left_recursion_continuation(pc);
                pc.restore_state(state);
                return result;
            }

        private:
            ParserT m_parser;
        };

        /**
         * A parser that uses another parser as a logical not test.
         * No side effects are created from the parsing.
         * @param ParserT the type of parser to make a test out of.
         */
        template <typename ParserT>
        class logical_not_parser : public parser<logical_not_parser<ParserT>> {
        public:
            /**
             * Constructor from copy of parser.
             * @param parser parser to create a loop out of.
             */
            logical_not_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor from parser; the object is moved.
             * @param parser parser to create a loop out of.
             */
            logical_not_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * Optimization for creating a logical not parser out of a logical not parser.
             * @@return reference to this.
             */
            const logical_not_parser<ParserT>& operator !() const {
                return *this;
            }

            /**
             * Parses the input with the help of the internal parser,
             * then restores the parse context to the state that it was
             * before the parsing.
             * @param pc parse context.
             * @return success if the internal parser returns failure, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse(pc);
                pc.restore_state(state);
                return result == parse_result::failure ? parse_result::success : parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse_left_recursion_base(pc);
                pc.restore_state(state);
                return result == parse_result::failure ? parse_result::success : parse_result::failure;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = m_parser.parse_left_recursion_continuation(pc);
                pc.restore_state(state);
                return result == parse_result::failure ? parse_result::success : parse_result::failure;
            }

        private:
            ParserT m_parser;
        };

        /**
         * Parser that uses a sequence of parsers for parsing.
         * In order for the sequence parser to succeed, all the internal parsers must succeed, in order.
         * @param ParserT sequence of parsers.
         */
        template <typename... ParserT>
        class sequence_parser : public parser<sequence_parser<ParserT...>> {
        public:
            /**
             * Constructor.
             * @param parsers parsers to use for the sequence.
             */
            sequence_parser(const std::tuple<ParserT...>& parsers)
                : m_parsers(parsers)
            {
            }

            /**
             * Constructor.
             * @param parsers parsers to use for the sequence; moved object.
             */
            sequence_parser(std::tuple<ParserT...>&& parsers)
                : m_parsers(std::move(parsers))
            {
            }

            /**
             * Returns the internal tuple of parsers.
             * @return the internal tuple of parsers.
             */
            std::tuple<ParserT...>& get_parsers() {
                return m_parsers;
            }

            /**
             * Parses a sequence.
             * @param pc parse context; upon failure, it's state is restored to the state before this call.
             * @return success if all parsers succeeded, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = parse_<0>(pc);
                if (result == parse_result::success) {
                    return parse_result::success;
                }
                pc.restore_state(state);
                return parse_result::failure;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = parse_left_recursion_base_<0>(pc);
                if (result == parse_result::success) {
                    return parse_result::success;
                }
                pc.restore_state(state);
                return parse_result::failure;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                auto state = pc.get_state();
                parse_result result = parse_left_recursion_continuation_<0>(pc);
                if (result == parse_result::success) {
                    return parse_result::success;
                }
                pc.restore_state(state);
                return parse_result::failure;
            }

        private:
            std::tuple<ParserT...> m_parsers;

            template <size_t Index>
            parse_result parse_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    parse_result result = std::get<Index>(m_parsers).parse(pc);
                    if (result == parse_result::failure) {
                        return parse_result::failure;
                    }
                    return parse_<Index + 1>(pc);
                }
                else {
                    return parse_result::success;
                }
            }

            template <size_t Index>
            parse_result parse_left_recursion_base_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    parse_result result = std::get<Index>(m_parsers).parse_left_recursion_base(pc);
                    if (result == parse_result::failure) {
                        return parse_result::failure;
                    }
                    return parse_left_recursion_base_<Index + 1>(pc);
                }
                else {
                    return parse_result::success;
                }
            }

            template <size_t Index>
            parse_result parse_left_recursion_continuation_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    parse_result result = std::get<Index>(m_parsers).parse_left_recursion_continuation(pc);
                    if (result == parse_result::failure) {
                        return parse_result::failure;
                    }
                    return parse_<Index + 1>(pc);
                }

                //sequence was parsed successfully
                else {
                    return parse_result::success;
                }
            }
        };

        /**
         * Parser that uses one or more parser as choices; if a parser succeeds, then parsing stops,
         * otherwise the next parser is chosen.
         * @param ParserT set of parsers to chose from.
         */
        template <typename... ParserT>
        class choice_parser : public parser<choice_parser<ParserT...>> {
        public:
            /**
             * Constructor.
             * @param parsers parsers to use for the sequence.
             */
            choice_parser(const std::tuple<ParserT...>& parsers)
                : m_parsers(parsers)
            {
            }

            /**
             * Constructor.
             * @param parsers parsers to use for the sequence; moved object.
             */
            choice_parser(std::tuple<ParserT...>&& parsers)
                : m_parsers(std::move(parsers))
            {
            }

            /**
             * Returns the internal tuple of parsers.
             * @return the internal tuple of parsers.
             */
            std::tuple<ParserT...>& get_parsers() {
                return m_parsers;
            }

            /**
             * Parses the input using the 1st available parser;
             * if that fails, then it choses the next parser, until no more parsers exist in this choice parser.
             * @param pc parse context.
             * @return success if at least one internal parser parses successfully, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                return parse_<0>(pc);
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return parse_left_recursion_base_<0>(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return parse_left_recursion_continuation_<0>(pc);
            }

        private:
            std::tuple<ParserT...> m_parsers;

            template <size_t Index>
            parse_result parse_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    auto state = pc.get_state();
                    parse_result result = std::get<Index>(m_parsers).parse(pc);
                    if (result == parse_result::success) {
                        return parse_result::success;
                    }
                    pc.restore_state(state);
                    return parse_<Index + 1>(pc);
                }
                else {
                    return parse_result::failure;
                }
            }

            template <size_t Index>
            parse_result parse_left_recursion_base_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    auto state = pc.get_state();
                    parse_result result = std::get<Index>(m_parsers).parse_left_recursion_base(pc);
                    if (result == parse_result::success) {
                        return parse_result::success;
                    }
                    pc.restore_state(state);
                    return parse_left_recursion_base_<Index + 1>(pc);
                }
                else {
                    return parse_result::failure;
                }
            }

            template <size_t Index>
            parse_result parse_left_recursion_continuation_(parse_context& pc) const {
                if constexpr (Index < sizeof...(ParserT)) {
                    auto state = pc.get_state();
                    parse_result result = std::get<Index>(m_parsers).parse_left_recursion_continuation(pc);
                    if (result == parse_result::success) {
                        return parse_result::success;
                    }
                    pc.restore_state(state);
                    return parse_left_recursion_continuation_<Index + 1>(pc);
                }
                else {
                    return parse_result::failure;
                }
            }
        };

        /**
         * A parser that adds a match to the parse context, if the internal parser parses successfully.
         * @param ParserT the internal parser to use.
         */
        template <typename ParserT>
        class match_parser : public parser<match_parser<ParserT>> {
        public:
            /**
             * Constructor.
             * @param parser the internal parser.
             * @param id the match id to add as a match if the given parser succeeds.
             */
            match_parser(const ParserT& parser, match_id_type id)
                : m_parser(parser)
                , m_id(id)
            {
            }

            /**
             * Constructor.
             * @param parser the internal parser; moved object.
             * @param id the match id to add as a match if the given parser succeeds.
             */
            match_parser(ParserT&& parser, match_id_type id)
                : m_parser(std::move(parser))
                , m_id(id)
            {
            }

            /**
             * If the internal parser parses successfully, then a match is added in the parse context.
             * @param pc parse context.
             * @return success if the internal parser parsed successfully, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    pc.add_match(m_id, start_position, end_position, end_match_index - start_match_index);
                }
                return result;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse_left_recursion_base(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    pc.add_match(m_id, start_position, end_position, end_match_index - start_match_index);
                }
                return result;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse_left_recursion_continuation(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    pc.add_match(m_id, start_position, end_position, end_match_index - start_match_index);
                }
                return result;
            }

        private:
            ParserT m_parser;
            match_id_type m_id;
        };

        /**
         * A parser that adds a custom match to the parse context, if the internal parser parses successfully.
         * @param ParserT the internal parser to use.
         * @param MatchFuncT type of function to invoke for creating a match id.
         */
        template <typename ParserT, typename MatchFuncT>
        class custom_match_parser : public parser<custom_match_parser<ParserT, MatchFuncT>> {
        public:
            /**
             * Constructor.
             * @param parser the internal parser.
             * @param func function to invoke for creating a match id.
             */
            custom_match_parser(const ParserT& parser, const MatchFuncT& func)
                : m_parser(parser)
                , m_func(func)
            {
            }

            /**
             * Constructor.
             * @param parser the internal parser; moved object.
             * @param id the match id to add as a match if the given parser succeeds.
             */
            custom_match_parser(ParserT&& parser, const MatchFuncT& func)
                : m_parser(std::move(parser))
                , m_func(func)
            {
            }

            /**
             * If the internal parser parses successfully, then a match is added in the parse context.
             * @param pc parse context.
             * @return success if the internal parser parsed successfully, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                auto state = pc.get_state();
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    try {
                        match_container_type child_matches(pc.get_matches().begin() + start_match_index, pc.get_matches().begin() + end_match_index);
                        match_id_type id = m_func(pc, child_matches);
                        pc.add_match(id, start_position, end_position, end_match_index - start_match_index, std::move(child_matches));
                    }
                    catch (...) {
                        pc.restore_state(state);
                        return parse_result::failure;
                    }
                }
                return result;
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                auto state = pc.get_state();
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse_left_recursion_base(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    try {
                        match_container_type child_matches(pc.get_matches().begin() + start_match_index, pc.get_matches.begin() + end_match_index);
                        match_id_type id = m_func(pc, child_matches);
                        pc.add_match(id, start_position, end_position, end_match_index - start_match_index, std::move(child_matches));
                    }
                    catch (...) {
                        pc.restore_state(state);
                        return parse_result::failure;
                    }
                }
                return result;
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                auto state = pc.get_state();
                auto [start_position, start_match_index] = pc.get_match_start();
                parse_result result = m_parser.parse_left_recursion_continuation(pc);
                if (result == parse_result::success) {
                    auto [end_position, end_match_index] = pc.get_match_end();
                    try {
                        match_container_type child_matches(pc.get_matches().begin() + start_match_index, pc.get_matches.begin() + end_match_index);
                        match_id_type id = m_func(pc, child_matches);
                        pc.add_match(id, start_position, end_position, end_match_index - start_match_index, std::move(child_matches));
                    }
                    catch (...) {
                        pc.restore_state(state);
                        return parse_result::failure;
                    }
                }
                return result;
            }

        private:
            ParserT m_parser;
            MatchFuncT m_func;
        };

        /**
         * A parser to use for debugging.
         * @param ParserT parser type to debug.
         */
        template <typename ParserT>
        class debug_parser : public parser<debug_parser<ParserT>> {
        public:
            /**
             * Constructor.
             * @param parser parser to debug.
             */
            debug_parser(const ParserT& parser)
                : m_parser(parser)
            {
            }

            /**
             * Constructor.
             * @param parser parser to debug; moved object.
             */
            debug_parser(ParserT&& parser)
                : m_parser(std::move(parser))
            {
            }

            /**
             * It simply invokes the internal parser.
             * A breakpoint can be added within this function order to debug the internal parser.
             * @param pc parse context.
             * @return success/failure of the internal parser.
             */
            parse_result parse(parse_context& pc) const {
                return m_parser.parse(pc);
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return m_parser.parse_left_recursion_base(pc);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return m_parser.parse_left_recursion_continuation(pc);
            }

        private:
            ParserT m_parser;
        };

        /**
         * A parser that holds a reference to a rule.
         * Used to allow recursive grammars.
         */
        class rule_reference_parser : public parser<rule_reference_parser> {
        public:
            /**
             * Constructor.
             * @param r rule to refer to.
             */
            rule_reference_parser(rule& r)
                : m_rule(r)
            {
            }

            /**
             * Invokes the rule for parsing.
             * @param pc parse context.
             * @return success if the rule parses successfully, failure otherwise.
             */
            parse_result parse(parse_context& pc) const {
                return pc.parse_rule(m_rule);
            }

            parse_result parse_left_recursion_base(parse_context& pc) const {
                return pc.parse_rule_left_recursion_base(m_rule);
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) const {
                return pc.parse_rule_left_recursion_continuation(m_rule);
            }

        private:
            rule& m_rule;
        };

        /**
         * A rule keeps a parsing expression hidden away behind an interface,
         * in order to allow recursive grammars.
         *
         * With rules, it is possible to write, for example:
         *
         *  rule a = 'x' >> a >> 'y';
         *
         * Rules also support left recursion:
         *
         *  rule a = a >> b
         *         | b;
         *
         * The class uses a shared_ptr to an internal interface to keep the grammar expression,
         * and therefore rules can be copied.
         */
        class rule {
        private:
            class parser_interface {
            public:
                virtual parse_result parse(parse_context& pc) const = 0;
                virtual parse_result parse_left_recursion_base(parse_context& pc) const = 0;
                virtual parse_result parse_left_recursion_continuation(parse_context& pc) const = 0;
                virtual const std::type_info& get_parser_type() const = 0;
            };

            template <typename ParserT>
            class parser_impl : public parser_interface {
            public:
                parser_impl(const ParserT& parser)
                    : m_parser(parser)
                {
                }

                parser_impl(ParserT&& parser)
                    : m_parser(std::move(parser))
                {
                }

                parse_result parse(parse_context& pc) const override {
                    return m_parser.parse(pc);
                }

                parse_result parse_left_recursion_base(parse_context& pc) const override {
                    return m_parser.parse_left_recursion_base(pc);
                }

                parse_result parse_left_recursion_continuation(parse_context& pc) const override {
                    return m_parser.parse_left_recursion_continuation(pc);
                }

                const std::type_info& get_parser_type() const override {
                    return typeid(ParserT);
                }

            private:
                ParserT m_parser;
            };

            struct left_recursion {
                class rule* const rule;
            };

        public:
            /**
             * The default constructor.
             * An empty rule is created.
             */
            rule() {
            }

            /**
             * The copy constructor.
             * @param r the source rule.
             */
            rule(const rule& r)
                : m_parser(r.m_parser)
            {
            }

            /**
             * The move constructor.
             * @param r the source rule.
             */
            rule(rule&& r)
                : m_parser(std::move(r.m_parser))
            {
            }

            /**
             * Constructor from grammar expression.
             * @param parser the parser to use internally.
             */
            template <typename ParserT>
            rule(ParserT&& parser)
                : m_parser(create_parser_impl(std::forward<ParserT>(parser)))
            {
            }

            /**
             * The copy assignment operator.
             * @param r the source rule.
             * @return reference to this.
             */
            rule& operator = (const rule& r) {
                m_parser = r.m_parser;
                return *this;
            }

            /**
             * The move assignment operator.
             * @param r the source rule.
             * @return reference to this.
             */
            rule& operator = (rule&& r) {
                m_parser = std::move(r.m_parser);
                return *this;
            }

            /**
             * The copy assignment operator from a grammar expression.
             * @param parser the parser to use internally.
             * @return reference to this.
             */
            template <typename ParserT>
            rule& operator = (ParserT&& parser) {
                m_parser = create_parser_impl(std::forward<ParserT>(parser));
                return *this;
            }

            /**
             * Returns pointer to this rule, since operator & is used for creating a logical and parser.
             * @return pointer to this rule.
             */
            rule* get_this_ptr() {
                return this;
            }

            /**
             * Creates a zero-or-more loop parser out of this rule.
             * @return a zero-or-more loop parser out of this rule.
             */
            zero_or_more_parser<rule_reference_parser> operator *() {
                return rule_reference_parser(*this);
            }

            /**
             * Creates a one-or-more loop parser out of this rule.
             * @return a one-or-more loop parser out of this rule.
             */
            one_or_more_parser<rule_reference_parser> operator +() {
                return rule_reference_parser(*this);
            }

            /**
             * Creates an optional parser out of this rule.
             * @return an optional parser out of this rule.
             */
            optional_parser<rule_reference_parser> operator -() {
                return rule_reference_parser(*this);
            }

            /**
             * Creates a logical-and parser out of this rule.
             * @return a logical-and parser out of this rule.
             */
            logical_and_parser<rule_reference_parser> operator &() {
                return rule_reference_parser(*this);
            }

            /**
             * Creates a logical-or parser out of this rule.
             * @return a logical-or parser out of this rule.
             */
            logical_not_parser<rule_reference_parser> operator !() {
                return rule_reference_parser(*this);
            }

            /**
             * Invokes the internal grammar expression for parsing of this rule.
             * @param pc parse context.
             * @return success if the internal parser succeeded, failure otherwise.
             */
            parse_result parse(parse_context& pc) {
                if (!pc.is_left_recursive_parse_position(this)) {
                    return parse_non_left_recursion(pc);
                }
                else {
                    throw left_recursion{ this };
                }
            }

            parse_result parse_left_recursion_base(parse_context& pc) {
                if (!pc.is_left_recursive_parse_position(this)) {
                    return parse_non_left_recursion(pc);
                }
                else {
                    return parse_result::failure;
                }
            }

            parse_result parse_left_recursion_continuation(parse_context& pc) {
                if (!pc.is_left_recursive_parse_position(this)) {
                    return parse_non_left_recursion(pc);
                }
                else {
                    return parse_result::success;
                }
            }

            const std::type_info& get_parser_type() const {
                return m_parser->get_parser_type();
            }

        private:
            std::shared_ptr<parser_interface> m_parser;

            template <typename ParserT>
            static auto create_parser_impl(ParserT&& parser) {
                using wrapper_type = decltype(make_parser_wrapper(std::forward<ParserT>(parser)));
                using parser_type = std::decay_t<wrapper_type>;
                return std::make_shared<parser_impl<parser_type>>(make_parser_wrapper(std::forward<ParserT>(parser)));
            }

            parse_result parse_(parse_context& pc) {
                pc.push_parse_position(this);
                try {
                    parse_result result = m_parser->parse(pc);
                    pc.pop_parse_position(this);
                    return result;
                }
                catch (...) {
                    pc.pop_parse_position(this);
                    throw;
                }
            }

            parse_result parse_left_recursion_base_(parse_context& pc) {
                pc.push_parse_position(this);
                parse_result result = m_parser->parse_left_recursion_base(pc);
                pc.pop_parse_position(this);
                return result;
            }

            parse_result parse_left_recursion_continuation_(parse_context& pc) {
                pc.push_parse_position(this);
                parse_result result = m_parser->parse_left_recursion_continuation(pc);
                pc.pop_parse_position(this);
                return result;
            }

            parse_result parse_non_left_recursion(parse_context& pc) {
                try {
                    return parse_(pc);
                }
                catch (left_recursion lr) {
                    return lr.rule == this ? parse_left_recursion(pc) : throw lr;
                }
            }

            parse_result parse_left_recursion(parse_context& pc) {
                auto match_start_position = pc.get_current_position();
                auto match_start_size = pc.get_matches().size();

                if (parse_left_recursion_base_(pc) == parse_result::failure) {
                    return parse_result::failure;
                }

                while (!pc.is_end_position()) {
                    pc.push_left_recursion_match_position(match_start_position, match_start_size, pc.get_current_position());
                    parse_result result = parse_left_recursion_continuation_(pc);
                    pc.pop_left_recursion_match_position();

                    if (result == parse_result::failure) {
                        break;
                    }
                }

                return parse_result::success;
            }
        };

        /**
         * Helper function for creating a terminal parser.
         * param value value to create a terminal parser from.
         * @return a terminal parser object.
         */
        template <typename ValueT>
        static auto terminal(ValueT&& value) {
            return make_parser_wrapper(std::forward<ValueT>(value));
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        template <typename ValueT>
        static terminal_set_parser<ValueT> one_of(std::initializer_list<ValueT> values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values null-terminated values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        template <typename ValueT>
        static terminal_set_parser<ValueT> one_of(const ValueT* values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        template <typename ValueT>
        static terminal_set_parser<ValueT> set(std::initializer_list<ValueT> values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values null-terminated values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        template <typename ValueT>
        static terminal_set_parser<ValueT> set(const ValueT* values) {
            return values;
        }

        /**
         * Helper function for creating a terminal range parser.
         * @param min min value of range.
         * @param max max value of range.
         * @return the appropriate range parser for the given range.
         */
        template <typename ValueT>
        static terminal_range_parser<ValueT> range(ValueT&& min, ValueT&& max) {
            return { std::forward<ValueT>(min), std::forward<ValueT>(max) };
        }

        /**
         * Helper function for creating a debug parser.
         * @param parser parser to debug.
         * @return a debug parser.
         */
        template <typename ParserT>
        static auto debug(ParserT&& parser) {
            return debug_parser(std::move(parser));
        }

        /**
         * Helper function for creating a function parser.
         * @param func function to create a function parser for.
         * @return the function parser.
         */
        template <typename FuncT>
        static auto function(FuncT&& func) {
            return function_parser<std::decay_t<FuncT>>(std::forward<FuncT>(func));
        }

        /**
         * Helper function that creates an error parser.
         * @param grammar grammar to use for parsing.
         * @param error_id id of error.
         * @param cont_func function to use for continuation; it must have the signature `parse_result(parse_context&)`.
         * @return an error parser.
         */
        template <typename ParserT, typename ContFuncT, std::enable_if_t<std::is_invocable_r_v<parse_result, ContFuncT, parse_context&>, bool> = true>
        static auto error(ParserT&& grammar, int error_id, ContFuncT&& cont_func) {
            return error_parser<ParserT, ContFuncT>(std::forward<ParserT>(grammar), error_id, std::forward<ContFuncT>(cont_func));
        }

        /**
         * Helper function that creates an error parser.
         * @param grammar grammar to use for parsing.
         * @param error_id id of error.
         * @param cont_token token to search for in order to continue from.
         * @return an error parser.
         */
        template <typename ParserT, typename ContTokenT, std::enable_if_t<!std::is_invocable_v<ContTokenT, parse_context&>, bool> = true>
        static auto error(ParserT&& grammar, int error_id, ContTokenT&& cont_token) {
            return error_parser(std::forward<ParserT>(grammar), error_id, [ct = std::forward<ContTokenT>(cont_token)](parse_context& pc) {
                while (pc.is_valid_position()) {
                    const bool found_cont_token = *pc.get_current_position() == ct;
                    pc.increment_position();
                    if (found_cont_token) {
                        return parse_result::success;
                    }
                }
                return parse_result::failure;
            });
        }

    private:
        template <typename T> struct is_parser_class {
            using base_type = std::decay_t<T>;
            static constexpr bool value = std::is_base_of_v<parser<base_type>, base_type> || std::is_same_v<base_type, rule>;
        };

        template <typename L, typename R> struct either_is_parser_class {
            static constexpr bool value = is_parser_class<L>::value || is_parser_class<R>::value;
        };

        template <typename L, typename R>
        static constexpr bool either_is_parser_class_v = either_is_parser_class<L, R>::value;

    public:
        /**
         * Creates a sequence out of two elements.
         * @param left left element.
         * @param right right element.
         * @return a sequence of parsers for the given elements.
         */
        template <typename Left, typename Right, std::enable_if_t<either_is_parser_class_v<Left, Right>, bool> = true>
        friend auto operator >> (Left&& left, Right&& right) {
            return sequence_parser(std::make_tuple(make_parser_wrapper(std::forward<Left>(left)), make_parser_wrapper(std::forward<Right>(right))));
        }

        /**
         * Creates a sequence out of two elements, one of them being a sequence.
         * @param left left element; sequence of elements.
         * @param right right element.
         * @return a sequence of parsers that contains the parsers of the left sequence and a parser for the right element.
         */
        template <typename... Left, typename Right>
        friend auto operator >> (sequence_parser<Left...>&& left, Right&& right) {
            return sequence_parser(std::tuple_cat(std::move(left.get_parsers()), std::make_tuple(make_parser_wrapper(std::forward<Right>(right)))));
        }

        /**
         * Creates a sequence out of two elements, one of them being a sequence.
         * @param left left element.
         * @param right element; sequence of elements.
         * @return a sequence of parsers that contains a parser for the left element and the parsers of the right sequence.
         */
        template <typename Left, typename... Right>
        friend auto operator >> (Left&& left, sequence_parser<Right...>&& right) {
            return sequence_parser(std::tuple_cat(std::make_tuple(make_parser_wrapper(std::forward<Left>(left))), std::move(right.get_parsers())));
        }

        /**
         * Creates a sequence out of two sequences.
         * @param left left sequence.
         * @param right right sequence.
         * @return a sequence of parsers that contains a parser for the parsers of the left sequence and the parsers of the right sequence.
         */
        template <typename... Left, typename... Right>
        friend auto operator >> (sequence_parser<Left...>&& left, sequence_parser<Right...>&& right) {
            return sequence_parser(std::tuple_cat(std::move(left.get_parsers()), std::move(right.get_parsers())));
        }

        /**
         * Creates a choice out of two elements.
         * @param left left element.
         * @param right right element.
         * @return a choice of parsers for the given elements.
         */
        template <typename Left, typename Right, std::enable_if_t<either_is_parser_class_v<Left, Right>, bool> = true>
        friend auto operator | (Left&& left, Right&& right) {
            return choice_parser(std::make_tuple(make_parser_wrapper(std::forward<Left>(left)), make_parser_wrapper(std::forward<Right>(right))));
        }

        /**
         * Creates a choice out of two elements, one of them being a choice.
         * @param left left element; choice of elements.
         * @param right right element.
         * @return a choice of parsers that contains the parsers of the left choice and a parser for the right element.
         */
        template <typename... Left, typename Right>
        friend auto operator | (choice_parser<Left...>&& left, Right&& right) {
            return choice_parser(std::tuple_cat(std::move(left.get_parsers()), std::make_tuple(make_parser_wrapper(std::forward<Right>(right)))));
        }

        /**
         * Creates a choice out of two elements, one of them being a choice.
         * @param left left element.
         * @param right element; choice of elements.
         * @return a choice of parsers that contains a parser for the left element and the parsers of the right choice.
         */
        template <typename Left, typename... Right>
        friend auto operator | (Left&& left, choice_parser<Right...>&& right) {
            return choice_parser(std::tuple_cat(std::make_tuple(make_parser_wrapper(std::forward<Left>(left))), std::move(right.get_parsers())));
        }

        /**
         * Creates a choice out of two choices.
         * @param left left choice.
         * @param right right choice.
         * @return a choice of parsers that contains a parser for the parsers of the left choice and the parsers of the right choice.
         */
        template <typename... Left, typename... Right>
        friend auto operator | (choice_parser<Left...>&& left, choice_parser<Right...>&& right) {
            return choice_parser(std::tuple_cat(std::move(left.get_parsers()), std::move(right.get_parsers())));
        }

        /**
         * Creates an exclusion pattern for a parser, as a sequence of the logical not of the right parser, and then the left parser.
         * @param left left element.
         * @param right right element.
         * @return a sequence of a logical not parser for the right element, followed by a parser for the left element.
         */
        template <typename Left, typename Right, std::enable_if_t<std::is_base_of_v<parser<Left>, Left> || std::is_base_of_v<parser<Right>, Right>, bool> = true>
        friend auto operator - (Left&& left, Right&& right) {
            return !make_parser_wrapper(std::forward<Right>(right)) >> make_parser_wrapper(std::forward<Left>(left));
        }

        /**
         * Adds a match function for the specific grammar.
         * @param left the grammar to add a match for.
         * @param right function that produces a match id, or match id to match.
         * @return a match parser for the given grammar and match function.
         */
        template <typename Left, typename Right>
        friend auto operator ->* (const parser<Left>& left, Right&& right) {
            if constexpr (std::is_invocable_r_v<match_id_type, Right, parse_context&, match_container_type&>) {
                return custom_match_parser(make_parser_wrapper(static_cast<const Left&>(left)), std::forward<Right>(right));
            }
            else {
                return match_parser(make_parser_wrapper(static_cast<const Left&>(left)), right);
            }
        }

        /**
         * Adds a match function for the specific rule.
         * @param left the rule to match.
         * @param right function that produces a match id, or match id to match.
         * @return a match parser for the given grammar and match function.
         */
        template <typename Right>
        friend auto operator ->* (rule& left, Right&& right) {
            if constexpr (std::is_invocable_r_v<match_id_type, Right, parse_context&, match_container_type&>) {
                return custom_match_parser(make_parser_wrapper(left), std::forward<Right>(right));
            }
            else {
                return match_parser(make_parser_wrapper(left), right);
            }
        }

        class ast_node;

        /**
         * AST node ptr type.
         * AST nodes are managed via reference counting.
         */
        using ast_node_ptr_type = std::shared_ptr<ast_node>;

        /**
         * In case a container of ast node ptrs is used as an input to a parser, this function is used to compare a match id
         * to an ast node's id.
         * @param ast to ast node.
         * @param id match id.
         * @return true if the ast node has the given id, false otherwise.
         */
        friend bool operator == (const ast_node_ptr_type& ast, match_id_type id) {
            return ast->get_id() == id;
        }

        /**
         * In case a container of ast node ptrs is used as an input to a parser, this function is used to compare a match id
         * to an ast node's id.
         * @param ast to ast node.
         * @param id match id.
         * @return true if the ast node does not have the given id, false otherwise.
         */
        friend bool operator != (const ast_node_ptr_type& ast, match_id_type id) {
            return ast->get_id() != id;
        }

        /**
         * In case a container of ast node ptrs is used as an input to a parser, this function is used to compare a match id
         * to an ast node's id.
         * @param ast to ast node.
         * @param id match id.
         * @return true if the ast node has the given id, false otherwise.
         */
        friend bool operator == (match_id_type id, const ast_node_ptr_type& ast) {
            return ast->get_id() == id;
        }

        /**
         * In case a container of ast node ptrs is used as an input to a parser, this function is used to compare a match id
         * to an ast node's id.
         * @param ast to ast node.
         * @param id match id.
         * @return true if the ast node does not have the given id, false otherwise.
         */
        friend bool operator != (match_id_type id, const ast_node_ptr_type& ast) {
            return ast->get_id() != id;
        }

        /**
         * Type of ast node ptr containr.
         */
        using ast_node_container_type = std::vector<ast_node_ptr_type>;

        /**
         * Abstract Syntax Tree node.
         */
        class ast_node : public std::enable_shared_from_this<ast_node> {
        public:
            /**
             * Constructor.
             * @param id id of node.
             * @param start_position start position into the source.
             * @param end_position end position into the source.
             */
            ast_node(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children)
                : m_id(id)
                , m_start_position(start_position)
                , m_end_position(end_position)
                , m_children(std::move(children))
            {
            }

            /**
             * Destructor is virtual because this class may be inherited.
             */
            virtual ~ast_node() {
            }

            /**
             * Returns the id of the node.
             * @return the id of the node.
             */
            match_id_type get_id() const {
                return m_id;
            }

            /**
             * Returns the start position of the source for this node.
             * @return the start position of the source for this node.
             */
            const iterator_type& get_start_position() const {
                return m_start_position;
            }

            /**
             * Returns the end position of the source for this node.
             * @return the end position of the source for this node.
             */
            const iterator_type& get_end_position() const {
                return m_end_position;
            }

            /**
             * Returns the parent ast node.
             * @return the parent ast node; possibly null.
             */
            ast_node_ptr_type get_parent() const {
                return m_parent.lock();
            }

            /**
             * Returns the children nodes.
             * @return the children nodes.
             */
            const ast_node_container_type& get_children() const {
                return m_children;
            }

            /**
             * Adds a child node.
             * @param child child node to add.
             */
            void add_child(const ast_node_ptr_type& child) {
                child->m_parent = this->shared_from_this();
                m_children.push_back(child);
            }

            /**
             * Adds a child node.
             * @param child child node to add; moved object.
             */
            void add_child(ast_node_ptr_type&& child) {
                child->m_parent = this->shared_from_this();
                m_children.push_back(std::move(child));
            }

            /**
             * Removes a child node.
             * @child child node to remove.
             */
            void remove_child(const ast_node_ptr_type& child) {
                child->m_parent.reset();
                m_children.erase(std::find(m_children.begin(), m_children.end(), child));
            }

            /**
             * Replaces a child node with another node.
             * @param old_child child to be replaced.
             * @param new_child new child.
             */
            void replace_child(const ast_node_ptr_type& old_child, const ast_node_ptr_type& new_child) {
                old_child->m_parent.reset();
                new_child->m_parent = this->shared_from_this();
                *std::find(m_children.begin(), m_children.end(), old_child) = new_child;
            }

            /**
             * Removes all children.
             */
            void remove_children() {
                for (const ast_node_ptr_type& child : m_children) {
                    child->m_parent.reset();
                }
                m_children.clear();
            }

            /**
             * Removes this node from its parent, if the parent is not null.
             */
            void detach_from_parent() {
                ast_node_ptr_type parent = get_parent();
                if (parent) {
                    parent->remove_child(this->shared_from_this());
                }
            }

            /**
             * Returns a view of the portion of the source that corresponds to this node.
             * @return a view of the portion of the source that corresponds to this node.
             */
            auto get_source() const {
                return get_source_helper(m_start_position, m_end_position);
            }

            /**
             * Prints this ast onto the given stream.
             * @param stream output stream.
             * @param depth current tree depth.
             * @param tabSize number of characters per tab.
             */
            template <typename OutStreamT>
            void print(OutStreamT& stream, size_t depth = 0, size_t tabSize = 4) const {
                for (size_t i = 0; i < depth * tabSize; ++i) {
                    stream << ' ';
                }
                stream << m_id;
                if (m_children.empty()) {
                    stream << ": " << get_source();
                }
                stream << '\n';
                for (const ast_node_ptr_type& child : m_children) {
                    child->print(stream, depth + 1, tabSize);
                }
            }

        private:
            match_id_type m_id;
            iterator_type m_start_position;
            iterator_type m_end_position;
            std::weak_ptr<ast_node> m_parent;
            ast_node_container_type m_children;
        };

        /**
         * Helper function for printing an AST tree onto an std stream.
         * @param stream output stream.
         * @param ast ast node to print.
         */
        template <typename Elem, typename Traits>
        friend std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, const ast_node_ptr_type& ast) {
            ast->print(stream);
            return stream;
        }

        /**
         * Function type for creating an ast node.
         * @param id match id.
         * @param start_position start position in source.
         * @param end_position end position in source.
         * @return pointer to the created ast node.
         */
        using create_ast_node_func_type = std::function<ast_node_ptr_type(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children)>;

        /**
         * Options passed to the parse function.
         */
        struct parse_options {
            /**
             * Function that allows to create custom ast nodes.
             */
            create_ast_node_func_type create_ast_node;

            /**
             * The default constructor.
             * The `create_ast_node` option is set to create an instance of `class ast_node`.
             */
            parse_options()
                : create_ast_node(&class_type::create_ast_node)
            {
            }
        };

        /**
         * The default `create_ast_node` function.
         * It creates an instance of `class ast_node`.
         * @param id match id.
         * @param start_position start position in source.
         * @param end_position end position in source.
         * @param children children nodes.
         * @return pointer to the created ast node.
         */
        static ast_node_ptr_type create_ast_node(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children) {
            return std::make_shared<ast_node>(id, start_position, end_position, std::move(children));
        }

        /**
         * Utility function that parses an input and creates AST nodes from the source.
         * @param input the source.
         * @param grammar the grammar to use for parsing.
         * @param options parse options.
         * @return a tuple of the following: success flag, ast nodes created, iterator where parsing stopped, errors.
         */
        template <typename GrammarT>
        static std::tuple<bool, ast_node_container_type, iterator_type, error_container_type> parse(SourceT& input, GrammarT&& grammar, const parse_options& options) {
            parse_context pc(input);
            const bool ok = grammar.parse(pc) == parse_result::success && pc.is_end_position();
            ast_node_container_type ast_nodes;
            create_ast(pc.get_matches(), ast_nodes, options.create_ast_node);
            return std::make_tuple(ok, std::move(ast_nodes), pc.get_unparsed_position(), pc.get_errors());
        }

        /**
         * Utility function that parses an input and creates AST nodes from the source.
         * Uses the default parse options.
         * @param input the source.
         * @param grammar the grammar to use for parsing.
         * @return a tuple of the following: success flag, ast nodes created, iterator where parsing stopped, errors.
         */
        template <typename GrammarT>
        static auto parse(SourceT& input, GrammarT&& grammar) {
            return parse(input, std::forward<GrammarT>(grammar), parse_options());
        }

    private:
        template <typename T> struct is_parser {
            static constexpr bool value = std::is_base_of_v<parser<T>, T>;
        };

        template <typename T> static constexpr bool is_parser_v = is_parser<T>::value;

        template <typename T> struct is_terminal_function {
            static constexpr bool value = std::is_invocable_v<T, terminal_value_type>;
        };

        template <typename T> static constexpr bool is_terminal_function_v = is_terminal_function<T>::value;

        template <typename T> struct is_parser_function {
            static constexpr bool value = std::is_invocable_r_v<parse_result, T, parse_context&>;
        };

        template <typename T> static constexpr bool is_parser_function_v = is_parser_function<T>::value;

        template <typename T> struct is_terminal_value {
            static constexpr bool value = !is_parser_v<T> && !is_terminal_function_v<T> && !is_parser_function_v<T>;
        };

        template <typename T> static constexpr bool is_terminal_value_v = is_terminal_value<T>::value;


        template <typename DerivedT>
        static const DerivedT& make_parser_wrapper(const parser<DerivedT>& t) {
            return static_cast<const DerivedT&>(t);
        }

        template <typename ValueT, std::enable_if_t<is_terminal_value_v<ValueT>, bool> = true>
        static terminal_value_parser<ValueT> make_parser_wrapper(ValueT val) {
            return val;
        }

        template <typename FuncT, std::enable_if_t<is_terminal_function_v<FuncT>, bool> = true>
        static terminal_function_parser<FuncT> make_parser_wrapper(FuncT&& func) {
            return std::forward<FuncT>(func);
        }

        template <typename ValueT, std::enable_if_t<is_terminal_value_v<ValueT>, bool> = true>
        static auto make_parser_wrapper(ValueT* str) {
            return terminal_string_parser<std::basic_string<ValueT>>(str);
        }

        template <typename ValueT, std::enable_if_t<is_terminal_value_v<ValueT>, bool> = true>
        static auto make_parser_wrapper(const ValueT* str) {
            return terminal_string_parser<std::basic_string<ValueT>>(str);
        }

        template <typename FuncT, std::enable_if_t<is_parser_function_v<FuncT>, bool> = true>
        static function_parser<FuncT> make_parser_wrapper(FuncT&& func) {
            return std::forward<FuncT>(func);
        }

        template <typename ValueT, typename CharTraits, typename Alloc>
        static auto make_parser_wrapper(std::basic_string<ValueT, CharTraits, Alloc> str) {
            return terminal_string_parser<std::basic_string<ValueT, CharTraits, Alloc>>(str);
        }

        static rule_reference_parser make_parser_wrapper(rule& r) {
            return r;
        }

        static void create_ast(const match& m, const create_ast_node_func_type& create_ast_node_func, ast_node_container_type& ast_nodes) {
            ast_node_container_type children;
            for (const match& cm : m.get_children()) {
                create_ast(cm, create_ast_node_func, children);
            }
            ast_nodes.push_back(create_ast_node_func(m.get_id(), m.get_start_position(), m.get_end_position(), std::move(children)));
        }

        /**
         * Helper function for creating multiple ast nodes from an array of matches.
         * Usually, there is no need to call this function directly, but it is provided as a public member
         * if a really custom parse function is required.
         * @param matches array of matches.
         * @param ast_nodes destination ast node container.
         * @param create_ast_node_func function to use for creating an ast node.
         */
        static void create_ast(const match_container_type& matches, ast_node_container_type& ast_nodes, const create_ast_node_func_type& create_ast_node_func) {
            for (const match& m : matches) {
                create_ast(m, create_ast_node_func, ast_nodes);
            }
        }

    private:
        //return a string view, if token is trivial, otherwise return an std::vector.
        template <class It> static auto get_source_helper(const It& begin, const It& end) {
            if constexpr (std::is_trivial_v<terminal_value_type>) {
                const terminal_value_type* start = &*begin;
                const size_t size = end - begin;
                return std::basic_string_view<terminal_value_type>(start, size);
            }
            else {
                return std::vector<terminal_value_type>(begin, end);
            }
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_ENGINE_HPP
