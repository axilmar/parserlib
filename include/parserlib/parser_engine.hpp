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
     *
     * @param MatchIdT type of id for matches; it defaults to 'int', if a special enumeration is not provided.
     */
    template <typename SourceT = std::string, typename MatchIdT = int> class parser_engine {
    public:
        class parse_context;
        class match;
        class rule_reference_parser;
        class rule;
        template <typename ParserT> class zero_or_more_parser;
        template <typename ParserT> class one_or_more_parser;
        template <typename ParserT> class optional_parser;
        template <typename ParserT> class logical_and_parser;
        template <typename ParserT> class logical_not_parser;
        template <typename ParserT> class match_parser;

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
            match(match_id_type match_id, const iterator_type& start_position, const iterator_type& end_position, match_container_type&& children)
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
             * Returns a view of the source that corresponds to this match.
             * @return a view of the source that corresponds to this match.
             */
            std::basic_string_view<terminal_value_type> get_source() const {
                const terminal_value_type* start = &*m_start_position;
                const size_t size = m_end_position - m_start_position;
                return { start, size };
            }

        private:
            match_id_type m_id;
            iterator_type m_start_position;
            iterator_type m_end_position;
            size_t m_child_count;
            match_container_type m_children;
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
         * Type of rule handler function.
         * Rule handlers are invoked after a rule is parsed.
         * @param pc the current parse context.
         * @param r the rule that was parsed.
         * @param result the result of the rule parsing.
         */
        using rule_handler_function_type = std::function<parse_result(parse_context& pc, rule& r, parse_result result, void* custom_data)>;

        /**
         * Type of map of rules to rule handlers.
         */
        using rule_handler_function_map_type = std::map<rule*, rule_handler_function_type>;

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

                state(const iterator_type& si, size_t matches_size)
                    : m_position(si)
                    , m_matches_size(matches_size)
                {
                }

                friend class parse_context;
            };

            /**
             * Constructor.
             * @param input the source; must be in scope while this parse context is in scope, since the matches produced
             *  create views on the source.
             * @param rule_handlers map of rules to rule handlers, to be invoked after a rule is parsed; the given container
             *  is empty upon return.
             */
            parse_context(source_type& input, const rule_handler_function_map_type& rule_handlers = rule_handler_function_map_type())
                : m_current_position(input.begin())
                , m_end_position(input.end())
                , m_rule_handler_functions(rule_handlers)
            {
            }

            /**
             * Constructor.
             * @param begin beginning of source.
             * @param end end of source.
             * @param rule_handlers map of rules to rule handlers, to be invoked after a rule is parsed; the given container
             *  is empty upon return.
             */
            parse_context(const iterator_type& begin, const iterator_type& end, const rule_handler_function_map_type& rule_handlers = rule_handler_function_map_type())
                : m_current_position(begin)
                , m_end_position(end)
                , m_rule_handler_functions(rule_handlers)
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
            }

            /**
             * Increments the current position by the given amount.
             * @param count number of characters to advance the iterator.
             * No checks are performed, for performance reasons.
             */
            void increment_position(size_t count) {
                m_current_position += count;
            }

            /**
             * Returns the current state of the parse context.
             * @return the current state of the parse context.
             */
            state get_state() const {
                return { m_current_position, m_matches.size() };
            }

            /**
             * Sets the state of the parse context.
             * @param st the state to set.
             */
            void restore_state(const state& st) {
                m_current_position = st.m_position;
                m_matches.resize(st.m_matches_size);
            }

            /**
             * Returns the container of the matches.
             * @return the container of the matches.
             */
            const match_container_type& get_matches() const {
                return m_matches;
            }

            /**
             * Returns the custom data attached to this parse context.
             * @return the custom data attached to this parse context.
             */
            void* get_custom_data() const {
                return m_custom_data;
            }

            /**
             * Sets the custom data attached to this parse context.
             * @param data custom data.
             */
            void set_custom_data(void* data) {
                m_custom_data = data;
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
            rule_handler_function_map_type m_rule_handler_functions;
            void* m_custom_data;

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
                parse_result result = r.parse(*this);
                auto it = m_rule_handler_functions.find(r.get_this_ptr());
                if (it != m_rule_handler_functions.end()) {
                    result = it->second(*this, r, result, m_custom_data);
                }
                return result;
            }

            parse_result parse_rule_left_recursion_base(rule& r) {
                return r.parse_left_recursion_base(*this);
            }

            parse_result parse_rule_left_recursion_continuation(rule& r) {
                return r.parse_left_recursion_continuation(*this);
            }

            template <typename ParserT> friend class match_parser;
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
        class terminal_value_parser : public parser<terminal_value_parser> {
        public:
            /**
             * Constructor.
             * @param value the terminal value.
             */
            terminal_value_parser(const terminal_value_type& value)
                : m_value(value)
            {
            }

            /**
             * Constructor.
             * @param value the terminal value.
             */
            terminal_value_parser(terminal_value_type&& value)
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
                if (pc.is_valid_position() && *pc.get_current_position() == m_value) {
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
            terminal_value_type m_value;
        };

        /**
         * A parser for strings.
         */
        class terminal_string_parser : public parser<terminal_string_parser> {
        public:
            /**
             * Constructor from null-terminated string.
             * @param string string to parse.
             */
            terminal_string_parser(const terminal_value_type* string)
                : m_string(string)
            {
            }

            /**
             * Constructor from string.
             * @param string string to parse.
             */
            terminal_string_parser(const terminal_string_type& string)
                : m_string(string)
            {
            }

            /**
             * Constructor from movable string.
             * @param string string to parse.
             */
            terminal_string_parser(terminal_string_type&& string)
                : m_string(std::move(string))
            {
            }

            /**
             * Constructor from list of values.
             * @param values list of values.
             */
            terminal_string_parser(std::initializer_list<terminal_value_type> values)
                : m_string(values)
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
                    if (*it_pc != *it_str) {
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
            terminal_string_type m_string;
        };

        /**
         * A parser for a set of terminal values.
         *
         * Internally, it uses a sorted flat array, and std::upper_bound to locate the appropriate set member.
         */
        class terminal_set_parser : public parser<terminal_set_parser> {
        public:
            /**
             * Constructor from terminal value types.
             * @param values values of the set.
             */
            terminal_set_parser(std::initializer_list<terminal_value_type> values)
                : m_set(values)
            {
                std::sort(m_set.begin(), m_set.end());
            }

            /**
             * Constructor from string of values.
             * @param values values of the set.
             */
            terminal_set_parser(const std::basic_string<terminal_value_type>& values)
                : m_set(values.begin(), values.end())
            {
                std::sort(m_set.begin(), m_set.end());
            }

            /**
             * Constructor from null-terminated string of values.
             * @param values values of the set.
             */
            terminal_set_parser(const terminal_value_type* values)
                : terminal_set_parser(std::basic_string<terminal_value_type>(values))
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
                    const terminal_value_type& val = *pc.get_current_position();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), val);
                    if (it != m_set.begin()) {
                        const terminal_value_type& pv = *std::prev(it);
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
            std::vector<terminal_value_type> m_set;
        };

        /**
         * A parser for a range of terminal values.
         */
        class terminal_range_parser : public parser<terminal_range_parser> {
        public:
            /**
             * Constructor.
             * @param min min value.
             * @param max max value.
             * @exception std::logic_error thrown if min is greater than max.
             */
            terminal_range_parser(const terminal_value_type& min, const terminal_value_type& max)
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
            terminal_range_parser(terminal_value_type&& min, terminal_value_type&& max)
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
                    const terminal_value_type& val = *pc.get_current_position();
                    if (val >= m_min && val <= m_max) {
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
            terminal_value_type m_min;
            terminal_value_type m_max;
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
             * @return success if all parsers suceeded, failure otherwise.
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
         * A rule keeps a parsing expession hidden away behind an interface,
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
             * @return success if the internal parser suceeded, failure otherwise.
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
                return std::make_shared<parser_impl<wrapper_type>>(make_parser_wrapper(std::forward<ParserT>(parser)));
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
         * Helper function for creating a terminal value parser.
         * param value value to create a terminal value parser from.
         * @return a terminal value parser object.
         */
        static terminal_value_parser terminal(terminal_value_type&& value) {
            return { std::move(value) };
        }

        /**
         * Helper function for creating a terminal string parser.
         * param value null-terminated string to create a terminal string parser from.
         * @return a terminal string parser object.
         */
        static terminal_string_parser terminal(const terminal_value_type* value) {
            return { terminal_string_type(value) };
        }

        /**
         * Helper function for creating a terminal string parser.
         * param value null-terminated string to create a terminal string parser from.
         * @return a terminal string parser object.
         */
        static terminal_string_parser terminal(std::initializer_list<terminal_value_type> values) {
            return { terminal_string_type(values) };
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        static terminal_set_parser one_of(std::initializer_list<terminal_value_type> values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values null-terminated values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        static terminal_set_parser one_of(const terminal_value_type* values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        static terminal_set_parser set(std::initializer_list<terminal_value_type> values) {
            return values;
        }

        /**
         * Helper function for creating a terminal set parser.
         * @param values null-terminated values to create a terminal set parser of.
         * @return a terminal set parser.
         */
        static terminal_set_parser set(const terminal_value_type* values) {
            return values;
        }

        /**
         * Helper function for creating a terminal range parser.
         * @param min min value of range.
         * @param max max value of range.
         * @return the appropriate range parser for the given range.
         */
        static terminal_range_parser range(terminal_value_type&& min, terminal_value_type&& max) {
            return { std::move(min), std::move(max) };
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

    private:
        template <typename T> struct is_parser {
            using base_type = std::decay_t<T>;
            static constexpr bool value = std::is_base_of_v<parser<base_type>, base_type> || std::is_same_v<base_type, rule>;
        };

        template <typename L, typename R> struct either_is_parser {
            static constexpr bool value = is_parser<L>::value || is_parser<R>::value;
        };

        template <typename L, typename R>
        static constexpr bool either_is_parser_v = either_is_parser<L, R>::value;

    public:
        /**
         * Creates a sequence out of two elements.
         * @param left left element.
         * @param right right element.
         * @return a sequence of parsers for the given elements.
         */
        template <typename Left, typename Right, std::enable_if_t<either_is_parser_v<Left, Right>, bool> = true>
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
        template <typename Left, typename Right, std::enable_if_t<either_is_parser_v<Left, Right>, bool> = true>
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
         * Adds a match for the specific grammar.
         * @param left the grammar to add a match for.
         * @param id the match id for this grammar.
         * @return a match parser for the given grammar and id.
         */
        template <typename Left>
        friend auto operator ->* (Left&& left, match_id_type id) {
            return match_parser(make_parser_wrapper(std::forward<Left>(left)), id);
        }

        class ast_node;

        /**
         * AST node ptr type.
         * AST nodes are managed via reference counting.
         */
        using ast_node_ptr_type = std::shared_ptr<ast_node>;

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
            ast_node(match_id_type id, const iterator_type& start_position, const iterator_type& end_position)
                : m_id(id)
                , m_start_position(start_position)
                , m_end_position(end_position)
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
            std::basic_string_view<terminal_value_type> get_source() const {
                const terminal_value_type* start = &*m_start_position;
                const size_t size = m_end_position - m_start_position;
                return { start, size };
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
        using create_ast_node_func_type = std::function<ast_node_ptr_type(match_id_type id, const iterator_type& start_position, const iterator_type& end_position)>;

        /**
         * Options passed to the parse function.
         */
        struct parse_options {
            /**
             * Map of rules to rule handlers.
             * Adding an entry to this map allows custom processing of rules.
             */
            rule_handler_function_map_type rule_handlers;

            /**
             * Function that allows to create custom ast nodes.
             */
            create_ast_node_func_type create_ast_node;

            /**
             * Pointer to custom data.
             */
            void* custom_data{ nullptr };

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
         * @return pointer to the created ast node.
         */
        static ast_node_ptr_type create_ast_node(match_id_type id, const iterator_type& start_position, const iterator_type& end_position) {
            return std::make_shared<ast_node>(id, start_position, end_position);
        }

        /**
         * Helper function for creating an ast node from a match.
         * It also creates nodes for children matches, recursively.
         * Usually, there is no need to call this function directly, but it is provided as a public member
         * if a really custom parse function is required.
         * @param m match to create an ast node for.
         * @param create_ast_node_func function to use for creating an ast node.
         * @return the created ast node.
         */
        static ast_node_ptr_type create_ast(const match& m, const create_ast_node_func_type& create_ast_node_func) {
            ast_node_ptr_type an = create_ast_node_func(m.get_id(), m.get_start_position(), m.get_end_position());
            for (const match& cm : m.get_children()) {
                an->add_child(create_ast(cm, create_ast_node_func));
            }
            return an;
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
                ast_node_ptr_type an = create_ast(m, create_ast_node_func);
                ast_nodes.push_back(an);
            }
        }

        /**
         * Utility function that parses an input and creates AST nodes from the source.
         * @param input the source.
         * @param grammar the grammar to use for parsing.
         * @param options parse options.
         * @return a tuple of the following: success flag, ast nodes created, iterator where parsing stopped.
         */
        template <typename GrammarT>
        static std::tuple<bool, ast_node_container_type, iterator_type> parse(SourceT& input, GrammarT&& grammar, const parse_options& options) {
            parse_context pc(input, options.rule_handlers);
            pc.set_custom_data(options.custom_data);
            const bool ok = grammar.parse(pc) == parse_result::success && pc.is_end_position();
            ast_node_container_type ast_nodes;
            create_ast(pc.get_matches(), ast_nodes, options.create_ast_node);
            return std::make_tuple(ok, std::move(ast_nodes), pc.get_current_position());
        }

        /**
         * Utility function that parses an input and creates AST nodes from the source.
         * Uses the default parse options.
         * @param input the source.
         * @param grammar the grammar to use for parsing.
         * @return a tuple of the following: success flag, ast nodes created, iterator where parsing stopped.
         */
        template <typename GrammarT>
        static std::tuple<bool, ast_node_container_type, iterator_type>  parse(SourceT& input, GrammarT&& grammar) {
            return parse(input, std::forward<GrammarT>(grammar), parse_options());
        }

    private:
        template <typename T>
        static T make_parser_wrapper(T t) {
            return t;
        }

        static terminal_value_parser make_parser_wrapper(terminal_value_type val) {
            return val;
        }

        static terminal_string_parser make_parser_wrapper(terminal_value_type* str) {
            return str;
        }

        static terminal_string_parser make_parser_wrapper(const terminal_value_type* str) {
            return str;
        }

        template <typename CharTraits, typename Alloc>
        static terminal_string_parser make_parser_wrapper(std::basic_string<terminal_value_type, CharTraits, Alloc> str) {
            return str;
        }

        static rule_reference_parser make_parser_wrapper(rule& r) {
            return r;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_ENGINE_HPP
