#ifndef PARSERLIB_HPP
#define PARSERLIB_HPP


#include <algorithm>
#include <cassert>
#include <cctype>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace parserlib {


    //trait for recognizing if T can be used in an std::basic_string.
    template <class T, class = std::void_t<>> struct is_valid_basic_string : std::false_type {};
    template <class T> struct is_valid_basic_string<T, std::void_t<std::basic_string<T>>> : std::true_type {};
    template <class T> constexpr bool is_valid_basic_string_v = is_valid_basic_string<T>::value;


    /**
     * A class that provides a static function `compare` used to compare symbols.
     */
    class default_symbol_comparator {
    public:
        /**
         * Compares two symbols.
         * Both symbols must be convertible to `intptr_t`.
         * @param left the left symbol to compare.
         * @param right the right symbol to compare.
         * @return the difference between the symbols (left minus right).
         */
        template <class L, class R>
        static intptr_t compare(const L& left, const R& right) noexcept {
            return static_cast<intptr_t>(left) - static_cast<intptr_t>(right);
        }
    };


    /**
     * An iterator wrapper that maintains line and column numbers.
     */ 
    template <class Iterator = std::string::const_iterator>
    class text_position_iterator : public Iterator {
    public:
        /**
         * The constructor.
         * @param iterator the default iterator value.
         */ 
        text_position_iterator(const Iterator& iterator = {}) : Iterator(iterator) {
        }

        /**
         * Increments, then returns a reference to this iterator.
         * It increases the column by 1.
         * @return a reference to this iterator.
         */ 
        text_position_iterator& operator ++() {
            Iterator::operator ++();
            ++m_column;
            return *this;
        }

        //Deleted to avoid confusion because only the prefix version increments the column.
        text_position_iterator operator ++(int) = delete;

        /**
         * Returns the current line value.
         * Lines start from 1.
         * @return the current line value.
         */ 
        size_t get_line() const noexcept {
            return m_line;
        }

        /**
         * Returns the column.
         * Coluns start from 1.
         * @return the column.
         */
        size_t get_column() const noexcept {
            return m_column;
        }

        /**
         * Increments the line and sets the column to 1.
         */ 
        void increment_line() noexcept {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_line{ 1 };
        size_t m_column{ 1 };
    };


    //empty because the default c++ iterators do not have column/line in them.
    template <class Iterator>
    void increment_iterator_line(Iterator& iterator) {
    }


    //invokes `increment_line()` on the iterator.
    template <class Iterator>
    void increment_iterator_line(text_position_iterator<Iterator>& iterator) {
        iterator.increment_line();
    }


    /**
     * A class that defines the parsing algorithms and the domain-specific language to create a PEG parser.
     * @param Iterator type of iterator that is used to iterate over.
     * @param MatchId type of match id.
     * @param ErrorId type of error id.
     * @param SymbolComparator type of class to use for comparing symbols.
     */ 
    template <class Iterator = typename std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parser {
    public:
        ///Iterator type.
        using iterator_type = Iterator;

        ///Match id type.
        using match_id_type = MatchId;

        ///Error id type.
        using error_id_type = ErrorId;

        ///Symbol comparator type.
        using symbol_comparator_type = SymbolComparator;

        ///this parser type.
        using parser_type = parser<Iterator, MatchId, ErrorId, SymbolComparator>;

        /**
         * Base class for source ranges.
         */ 
        class source_range {
        public:
            ///origin parser type.
            using origin_parser_type = parser_type;

            /**
             * The constructor.
             * @param begin iterator that the source starts from.
             * @param end iterator that the source ends to.
             */ 
            source_range(const Iterator& begin, const Iterator& end)
                : m_begin(begin)
                , m_end(end)
            {
            }

            /**
             * Returns the begin iterator.
             * @return the begin iterator.
             */ 
            const Iterator& begin() const noexcept {
                return m_begin;
            }

            /**
             * Returns the end iterator.
             * @return the end iterator.
             */ 
            const Iterator& end() const noexcept {
                return m_end;
            }

            /**
             * Returns the source either as a basic string or a vector,
             * depending on if the value type of the iterator can be used
             * in an std::basic_string.
             * @return a container with the part of the source the range includes.
             */ 
            auto get_source() const {
                return get_source_impl(m_begin, m_end);
            }

            /**
             * Returns the origin source either as a basic string or a vector,
             * depending on if the value type of the origin source iterator can be used
             * in an std::basic_string.
             * The origin source is the source that was parsed into an array of matches,
             * and the array of matches were again parsed to produce this source range.
             * @return a container with the part of the source the range includes.
             */ 
            auto get_origin_source() const {
                return get_origin_source_impl(m_begin, m_end);
            }

        private:
            Iterator m_begin;
            Iterator m_end;

            template <class It> 
            static auto get_source_impl(const It& begin, const It& end) {
                using value_type = std::decay_t<typename It::value_type>;
                if constexpr (is_valid_basic_string_v<value_type>) {
                    return std::basic_string<value_type>{ begin, end };
                }
                else {
                    return std::vector<value_type>{ begin, end };
                }
            }

            template <class T, class = std::void_t<>> struct is_match_instance : std::false_type {};
            template <class T> struct is_match_instance<T, std::void_t<typename T::match_parser_type>> : std::true_type {};

            template <class T>
            static constexpr bool is_match_instance_v = is_match_instance<T>::value;

            template <class It> 
            static auto get_origin_source_impl(const It& begin, const It& end) {
                using value_type = std::decay_t<typename It::value_type>;
                if constexpr (is_match_instance_v<value_type>) {
                    return get_origin_source_impl(begin->begin(), std::prev(end)->end());
                }
                else {
                    return get_source_impl(begin, end);
                }
            }
        };

        /**
         * An instance of a match.
         */ 
        class match_instance : public source_range {
        public:
            ///parser type for this match.
            using match_parser_type = parser_type;

            /**
             * The constructor.
             * @param begin the begin iterator.
             * @param end the end iterator.
             * @param id the match id.
             * @param children the child matches.
             */ 
            match_instance(const Iterator& begin = {}, const Iterator& end = {}, MatchId id = {}, std::vector<match_instance>&& children = {})
                : source_range(begin, end)
                , m_id(id)
                , m_children(std::move(children))
            {
            }

            /**
             * Returns the match id.
             * @return the match id.
             */ 
            MatchId get_id() const noexcept {
                return m_id;
            }

            /**
             * Returns the child matches.
             * @return the child matches.
             */ 
            const std::vector<match_instance>& get_matches() const noexcept {
                return m_children;
            }

            /**
             * Returns the child matches.
             * @return the child matches.
             */ 
            const std::vector<match_instance>& get_children() const noexcept {
                return m_children;
            }

            /**
             * Allows comparison of match to token id.
             * @return the id as an intptr_t.
             */ 
            operator intptr_t () const noexcept {
                return static_cast<intptr_t>(m_id);
            }

        private:
            MatchId m_id;
            std::vector<match_instance> m_children;
        };

        /**
         * An instance of an error.
         */ 
        class error_instance : public source_range {
        public:
            /**
             * The constructor.
             * @param begin the begin iterator.
             * @param end the end iterator.
             * @param id the error id.
             */ 
            error_instance(const Iterator& begin = {}, const Iterator& end = {}, ErrorId id = {})
                : source_range(begin, end)
                , m_id(id)
            {
            }

            /**
             * Returns the error id.
             * @return the error id.
             */ 
            ErrorId get_id() const noexcept {
                return m_id;
            }

        private:
            ErrorId m_id;
        };

        /**
         * A class that represents the parse data to use while parsing.
         */ 
        class parse_context {
        public:
            /**
             * An opaque type used to hold the state of a parse context.
             * It can be used to revert a parse context to a previous state.
             */ 
            struct state {
            private:
                Iterator m_iterator;
                size_t m_match_position_count;
                size_t m_match_count;

                state(const Iterator& it, size_t match_position_count, size_t match_count)
                    : m_iterator(it)
                    , m_match_position_count(match_position_count)
                    , m_match_count(match_count)
                {
                }

                friend class parse_context;
            };

            /**
             * The constructor.
             * @param begin the begin iterator.
             * @param end the end iterator.
             */ 
            parse_context(const Iterator& begin, const Iterator& end)
                : m_iterator(begin)
                , m_end_iterator(end)
            {
            }

            /**
             * Constructor from container.
             * @param container the container that contains the source to parse.
             */ 
            template <class Container>
            parse_context(const Container& container)
                : parse_context(container.begin(), container.end()) {
            }

            /**
             * Records and returns the current state of the parse context.
             * @return the current state of the parse context.
             */ 
            state get_state() const noexcept {
                return { m_iterator, m_match_positions.size(), m_matches.size() };
            }

            /**
             * Sets the current state of the parse context.
             * If the given state does not reflect a previous state of this parse context,
             * the result is undefined.
             * @param st the state to set.
             */ 
            void set_state(const state& st) {
                m_iterator = st.m_iterator;
                m_match_positions.resize(st.m_match_position_count);
                m_matches.resize(st.m_match_count);
            }

            /**
             * Returns the current iterator, i.e. the parse position.
             * @return the current iterator.
             */ 
            const Iterator& get_iterator() const noexcept {
                return m_iterator;
            }

            /**
             * Sets the current iterator, i.e. the parse position.
             * @param iterator the current iterator.
             */ 
            void set_iterator(const Iterator& iterator) {
                m_iterator = iterator;
            }

            /**
             * Increments the iterator.
             */ 
            void increment_iterator() {
                ++m_iterator;
            }

            /**
             * Increments the iterator line.
             * If the iterator does not support lines, then this is an empty call.
             */ 
            void increment_line() {
                increment_iterator_line(m_iterator);
            }

            /**
             * Returns the end iterator.
             * @return the end iterator.
             */ 
            const Iterator& get_end_iterator() const noexcept {
                return m_end_iterator;
            }

            /**
             * Checks if the current iterator has not reached the end iterator yet.
             * @return true if the current iterator has not reached the end iterator, false otherwise.
             */ 
            bool is_valid_iterator() const noexcept {
                return m_iterator != m_end_iterator;
            }

            /**
             * Checks if the current iterator has reached the end iterator.
             * @return true if the current iterator has reached the end iterator, false otherwise.
             */ 
            bool is_end_iterator() const noexcept {
                return m_iterator == m_end_iterator;
            }

            /**
             * Returns the matches.
             * @return the matches.
             */ 
            const std::vector<match_instance>& get_matches() const noexcept {
                return m_matches;
            }

            /**
             * Records a match position in an internal stack.
             */ 
            void push_match_position() {
                m_match_positions.push_back(match_position{ m_iterator, m_matches.size() });
            }

            /**
             * Pops the top match position from the internal stack.
             */ 
            void pop_match_position() {
                assert(m_match_positions.size() > 0);
                m_match_positions.pop_back();
            }

            /**
             * Adds a match.
             * The match range is from the last match position recorded up to the current iterator position.
             * @param id the match id.
             */ 
            void add_match(MatchId id) {
                assert(m_match_positions.size() > 0);
                const match_position& mp = m_match_positions.back();
                std::vector<match_instance> children{std::make_move_iterator(m_matches.begin() + mp.match_count), std::make_move_iterator(m_matches.end())};
                m_matches.resize(mp.match_count);
                m_matches.push_back(match_instance{ mp.iterator, m_iterator, id, std::move(children) });
            }

            /**
             * Returns the errors.
             * @return the errors.
             */ 
            const std::vector<error_instance>& get_errors() const noexcept {
                return m_errors;
            }

            /**
             * Adds an error.
             * @param begin the begin iterator.
             * @param end the end iterator.
             * @param id the error id.
             */ 
            void add_error(const Iterator& begin, const Iterator& end, ErrorId id) {
                m_errors.push_back(error_instance{ begin, end, id });
            }

            /**
             * Shortcut function for comparing two symbols, using the supplied symbol comparator.
             * @param left the left symbol to compare.
             * @param right the right symbol to compare.
             * @return the difference between the symbols (left minus right).
             */ 
            template <class L, class R>
            static intptr_t compare(const L& left, const R& right) noexcept {
                return SymbolComparator::compare(left, right);
            }

        private:
            struct match_position {
                Iterator iterator;
                size_t match_count;
            };

            Iterator m_iterator;
            const Iterator m_end_iterator;
            std::vector<match_position> m_match_positions;
            std::vector<match_instance> m_matches;
            std::vector<error_instance> m_errors;
        };

        /**
         * Base class for parse nodes.
         * Parse nodes form parse trees and are used to implement parsing algorithms.
         */ 
        class parse_node {
        public:
            virtual ~parse_node() {}

            /**
             * Interface for parsing.
             * If parsing fails, then the node is responsible for setting the given parse context to a good state.
             * @param pc the parse context to use for parsing.
             * @return true if parsing succeeds, false otherwise.
             */ 
            virtual bool parse(parse_context& pc) const = 0;

            /**
             * Returns the name of the parse node.
             * @return the name of the parse node.
             */ 
            const std::string& get_name() const noexcept {
                return m_name;
            }

            /**
             * Sets the name of this parse node.
             * @param name the parse node name.
             */ 
            void set_name(const std::string& name) {
                m_name = name;
            }

        private:
            const std::string m_name;
        };

        /**
         * A parse node that parses a single symbol.
         * @param Symbol type of symbol to parse; usually a character (for character parsers)
         *  or an enum type (for non-character parsers).
         */ 
        template <class Symbol>
        class symbol_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param symbol the symbol to parse.
             */ 
            symbol_parse_node(Symbol symbol) : m_symbol(symbol) {
            }

            /**
             * Returns the symbol to parse.
             * @return the symbol to parse.
             */ 
            Symbol get_symbol() const noexcept {
                return m_symbol;
            }

            /**
             * Parses the symbol.
             * @param pc the parse context.
             * @return true if parsing succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    if (parse_context::compare(source_symbol, m_symbol) == 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            Symbol m_symbol;
        };

        /**
         * A parse node that parses a string.
         * @param Char type of character to parse.
         */ 
        template <class Char>
        class string_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param string the string to parse.
             */ 
            string_parse_node(const std::basic_string<Char>& string) : m_string(string) {
            }

            /**
             * Returns the string to parse.
             * @return the string to parse.
             */ 
            const std::basic_string<Char>& get_string() const noexcept {
                return m_string;
            }

            /**
             * Parses the string.
             * @param pc the parse context.
             * @return true if parsing succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                auto string_it = m_string.begin();
                auto source_it = pc.get_iterator();
                for (;;) {
                    if (string_it == m_string.end()) {
                        pc.set_iterator(source_it);
                        return true;
                    }
                    if (source_it == pc.get_end_iterator()) {
                        break;
                    }
                    const auto& string_symbol = *string_it;
                    const auto& source_symbol = *source_it;
                    if (parse_context::compare(source_symbol, string_symbol) != 0) {
                        break;
                    }
                    ++string_it;
                    ++source_it;
                }
                return false;
            }

        private:
            std::basic_string<Char> m_string;
        };

        /**
         * A parse node that parses a single symbol out of a set.
         * @param Symbol type of symbol to parse.
         */ 
        template <class Symbol>
        class set_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param set the set of symbols.
             */ 
            set_parse_node(const std::vector<Symbol>& set) : m_set(set) {
                std::sort(m_set.begin(), m_set.end());
            }

            /**
             * Returns the set of symbols.
             * @return the set of symbols.
             */ 
            const std::vector<Symbol>& get_set() const noexcept {
                return m_set;
            }

            /**
             * Parses a symbol from the set.
             * It uses binary search, hence it is fast even for large sets.
             * @param pc the parse context.
             * @return true if parsing succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), source_symbol, [](const auto& l, const auto& r) { return parse_context::compare(l, r) < 0; });
                    if (it != m_set.begin()) {
                        --it;
                        const auto& set_symbol = *it;
                        if (parse_context::compare(source_symbol, set_symbol) == 0) {
                            pc.increment_iterator();
                            return true;
                        }
                    }
                }
                return false;
            }

        private:
            std::vector<Symbol> m_set;
        };

        /**
         * A parse node that parses a symbol from within a range of symbols.
         * @param Symbol type of symbol to parse.
         */ 
        template <class Symbol>
        class range_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param min the min value of the range.
             * @param max the max value of the range.
             */ 
            range_parse_node(Symbol min, Symbol max) : m_min(min), m_max(max) {
                assert(min <= max);
            }

            /**
             * Returns the min value.
             * @return the min value.
             */
            Symbol get_min() const noexcept {
                return m_min;
            }

            /**
             * Returns the max value.
             * @return the max value.
             */
            Symbol get_max() const noexcept {
                return m_max;
            }

            /**
             * Parses a symbol from the range.
             * @param pc the parse context.
             * @return true if parsing succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    if (parse_context::compare(source_symbol, m_min) >= 0 && parse_context::compare(source_symbol, m_max) <= 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            Symbol m_min, m_max;
        };

        /**
         * Type of the parse node ptr.
         */ 
        using parse_node_ptr_type = std::shared_ptr<parse_node>;

        /**
         * A parse node that repeats another parse node 0 or more times.
         */ 
        class loop_0_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            loop_0_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node repeatedly, until no more parsing can be done.
             * @param pc the parse context.
             * @return true.
             */ 
            bool parse(parse_context& pc) const override {
                while (m_child->parse(pc) && pc.is_valid_iterator()) {
                }
                return true;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that repeats another parse node 1 or more times.
         */ 
        class loop_1_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            loop_1_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node once, then if that succeeds, 
             * it invokes he child repeatedly until no more parsing can be done.
             * @param pc the parse context.
             * @return true if the child succeeds at least once, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (m_child->parse(pc)) {
                    while (m_child->parse(pc) && pc.is_valid_iterator()) {
                    }
                    return true;
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that repeats another parse node a specific amount of times.
         */ 
        class loop_n_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             * @param count number of times to repeat the child.
             */ 
            loop_n_parse_node(const parse_node_ptr_type& child, size_t count) : m_child(child), m_count(count) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node the specified amount of times.
             * @param pc the parse context.
             * @return true if the child succeeds to parse the specified amount of times, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                for (size_t count = 0; count < m_count; ++count) {
                    if (!m_child->parse(pc)) {
                        pc.set_state(prev_state);
                        return false;
                    }
                }
                return true;
            }

        private:
            parse_node_ptr_type m_child;
            size_t m_count;
        };

        /**
         * A parse node that makes another parse node optional.
         */ 
        class optional_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            optional_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node once.
             * @param pc the parse context.
             * @return true.
             */ 
            bool parse(parse_context& pc) const override {
                m_child->parse(pc);
                return true;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that uses another parse node as a positive logical predicate.
         */ 
        class logical_and_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            logical_and_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node once.
             * After the invocation, the state of the parse context is restored
             * to the one before the invocation.
             * @param pc the parse context.
             * @return the result of the child parse node.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                const bool result = m_child->parse(pc);
                pc.set_state(prev_state);
                return result;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that uses another parse node as a negative logical predicate.
         */ 
        class logical_not_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            logical_not_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child node once.
             * After the invocation, the state of the parse context is restored
             * to the one before the invocation.
             * @param pc the parse context.
             * @return the opposite result of the child parse node.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                const bool result = !m_child->parse(pc);
                pc.set_state(prev_state);
                return result;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that invokes a series of parse nodes.
         * All parse nodes must succeed in order for the sequence to succeed.
         */ 
        class sequence_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param children the child nodes.
             */ 
            sequence_parse_node(std::vector<parse_node_ptr_type>&& children) : m_children(std::move(children)) {
            }

            /**
             * Returns the child nodes.
             * @return the child nodes.
             */ 
            const std::vector<parse_node_ptr_type>& get_children() const noexcept {
                return m_children;
            }

            /**
             * Invokes the given children, one by one, in the order specified in the constructor, 
             * each child being passed the same parse context. 
             * If a child fails to parse, then the loop stops and the parse context state is restored
             * to the initial state.
             * @param pc the parse context.
             * @return true if all the children succeeded, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                for (const parse_node_ptr_type& child : m_children) {
                    if (!child->parse(pc)) {
                        pc.set_state(prev_state);
                        return false;
                    }
                }
                return true;
            }

        private:
            std::vector<parse_node_ptr_type> m_children;
        };

        /**
         * A parse node that invokes a series of parse nodes.
         * At least one parse node must succeed in order for the choice to succeed.
         */ 
        class choice_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param children the child nodes.
             */ 
            choice_parse_node(std::vector<parse_node_ptr_type>&& children) : m_children(std::move(children)) {
            }

            /**
             * Returns the child nodes.
             * @return the child nodes.
             */ 
            const std::vector<parse_node_ptr_type>& get_children() const noexcept {
                return m_children;
            }

            /**
             * Invokes the given children, one by one, in the order specified in the constructor
             * each child being passed the initial parse context. 
             * If a child succeeds, then the algorithm stops.
             * if all children fail to parse, then the loop stops and the parse context state is restored
             * to the initial state.
             * @param pc the parse context.
             * @return true if a child succeeded, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                for (const parse_node_ptr_type& child : m_children) {
                    const typename parse_context::state prev_state = pc.get_state();
                    if (child->parse(pc)) {
                        return true;
                    }
                    pc.set_state(prev_state);
                }
                return false;
            }

        private:
            std::vector<parse_node_ptr_type> m_children;
        };

        /**
         * A parse node that adds a match to a parse context
         * if another node succeeds in parsing.
         */ 
        class match_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             * @param id the match id.
             */ 
            match_parse_node(const parse_node_ptr_type& child, MatchId id)
                : m_child(child)
                , m_id(id)
            {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Returns the match id.
             * @return the match id.
             */ 
            MatchId get_id() const noexcept {
                return m_id;
            }

            /**
             * Invokes the child node once, then if that succeeds, 
             * it adds a match to the given parse context.
             * @param pc the parse context.
             * @return true if the child succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                pc.push_match_position();
                if (m_child->parse(pc)) {
                    pc.add_match(m_id);
                    pc.pop_match_position();
                    return true;
                }
                pc.pop_match_position();
                return false;
            }

        private:
            parse_node_ptr_type m_child;
            MatchId m_id;
        };

        /**
         * A parse node that implements the initial action of a match,
         * which is to record the current parse position as the parse position
         * of the next match.
         */ 
        class begin_match_parse_node : public parse_node {
        public:
            /**
             * Pushes the current parse position into the internal stack of the parse context.
             * @param pc the parse context.
             * @return true.
             */ 
            bool parse(parse_context& pc) const override {
                pc.push_match_position();
                return true;
            }
        };

        /**
         * A parse node that adds a match to a parse context.
         * It can be used more than once in a loop, allowing left-associative parsing.
         */ 
        class add_match_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param id the match id.
             */ 
            add_match_parse_node(MatchId id) : m_id(id) {
            }

            /**
             * Returns the match id.
             * @return the match id.
             */ 
            MatchId get_id() const noexcept {
                return m_id;
            }

            /**
             * Adds a match to the given parse context.
             * @return true.
             */ 
            bool parse(parse_context& pc) const override {
                pc.add_match(m_id);
                return true;
            }

        private:
            MatchId m_id;
        };

        /**
         * A parse node that implements the final action of a match,
         * which is to pop the top match position from a parse context's internal stack.
         */ 
        class end_match_parse_node : public parse_node {
        public:
            /**
             * Pops the top match position from the internal stack of the parse context.
             * @param pc the parse context.
             * @return true.
             */ 
            bool parse(parse_context& pc) const override {
                pc.pop_match_position();
                return true;
            }
        };

        /**
         * A parse node that parses any symbol.
         */ 
        class any_parse_node : public parse_node {
        public:
            /**
             * If the parse position is valid, then it increments the parse iterator.
             * @param pc the parse context.
             * @return true if the parse position is valid, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    pc.increment_iterator();
                    return true;
                }
                return false;
            }
        };

        /**
         * A parse node that checks if the end of input has been reached.
         */ 
        class end_parse_node : public parse_node {
        public:
            /**
             * Checks if the end of input has been reached.
             * @param pc the parse context.
             * @return true if the end of input has been reached, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                return pc.is_end_iterator();
            }
        };

        /**
         * A parse node that invokes the `increment_line()` method
         * of the current iterator in order to increment the line,
         * if another parse node parses successfully.
         */ 
        class newline_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            newline_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * If the child parses successfully, the function `pc.increment_line()` is invoked.
             * @param pc the parse context.
             * @return true if the child succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                if (m_child->parse(pc)) {
                    pc.increment_line();
                    return true;
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that invokes a functor or function for parsing.
         * It can be used with objects, including lambdas, and pointers to functions.
         * @param F type of parsing.
         */ 
        template <class F>
        class function_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param function the functor/function to invoke.
             */ 
            function_parse_node(const F& function) : m_function(function) {
            }

            /**
             * Returns the function.
             * @return the function.
             */ 
            const F& get_function() const noexcept {
                return m_function;
            }

            /**
             * Invokes the function with the given parse context.
             * @param pc the parse context.
             * @return the result of the function invocation.
             */ 
            bool parse(parse_context& pc) const override {
                return m_function(pc);
            }

        private:
            F m_function;
        };

        /**
         * A parse node that adds an error to a parse context.
         */ 
        class error_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param skip_node the node to use for skipping to the end of the error.
             * @param id the error id.
             */ 
            error_parse_node(const parse_node_ptr_type& skip_node, ErrorId id)
                : m_skip_node(skip_node), m_id(id)
            {
            }

            /**
             * Returns the error id.
             * @return the error id.
             */ 
            ErrorId get_id() const noexcept {
                return m_id;
            }

            /**
             * Returns the skip node.
             * @return the skip node.
             */ 
            const parse_node_ptr_type& get_skip_node() const noexcept {
                return m_skip_node;
            }

            /**
             * Invokes the skip node. 
             * If the skip node succeeds in parsing, then an error is added
             * to the given parse context.
             * The error range starts from the parse position when this is invoked
             * and ends to the parse position of the parse context after the skip node is invoked.
             * @param pc the parse context.
             * @return true if the skip node suceeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                const auto begin_iterator = pc.get_iterator();
                if (m_skip_node->parse(pc)) {
                    pc.add_error(begin_iterator, pc.get_iterator(), m_id);
                    return true;
                }
                return false;
            }

        private:
            parse_node_ptr_type m_skip_node;
            ErrorId m_id;
        };

        /**
         * A parse node that skips the source until the child parse node succeeds.
         * Upon return, the current parse position is set to the parse position
         * just before the successful child parsing.
         */ 
        class skip_before_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            skip_before_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child until the child succeeds or until the source is exhausted.
             * If the child succeeds, then upon return the parse position points to the position
             * the parse context had before the child was invoked.
             * Matches and errors placed in the parse context while skipping source are ignored.
             * @param pc the parse context.
             * @return true if the child succeeded, false if the source is exhausted.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                while (pc.is_valid_iterator()) {
                    const auto before_parse_iterator = pc.get_iterator();
                    if (m_child->parse(pc)) {
                        pc.set_state(prev_state);
                        pc.set_iterator(before_parse_iterator);
                        return true;
                    }
                    pc.increment_iterator();
                }
                pc.set_state(prev_state);
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that skips the source until the child parse node succeeds.
         * Upon return, the current parse position is set to the parse position
         * just after the successful child parsing.
         */ 
        class skip_after_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            skip_after_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Invokes the child until the child succeeds or until the source is exhausted.
             * If the child succeeds, then upon return the parse position points to the position
             * the parse context had after the child was invoked.
             * Matches and errors placed in the parse context while skipping source are ignored.
             * @param pc the parse context.
             * @return true if the child succeeded, false if the source is exhausted.
             */ 
            bool parse(parse_context& pc) const override {
                const typename parse_context::state prev_state = pc.get_state();
                while (pc.is_valid_iterator()) {
                    if (m_child->parse(pc)) {
                        const auto after_parse_iterator = pc.get_iterator();
                        pc.set_state(prev_state);
                        pc.set_iterator(after_parse_iterator);
                        return true;
                    }
                    pc.increment_iterator();
                }
                pc.set_state(prev_state);
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        /**
         * A parse node that can be used to debug parsing.
         */ 
        class debug_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             * @param tag the debug tag.
             */ 
            debug_parse_node(const parse_node_ptr_type& child, const std::string& tag) : m_child(child), m_tag(tag) {
            }

            /**
             * Returns the child node.
             * @return the child node.
             */ 
            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            /**
             * Returns the debug tag.
             * @return the debug tag.
             */ 
            const std::string& get_tag() const noexcept {
                return m_tag;
            }

            /**
             * It invokes the child node.
             * A breakpoint can be placed here to allow for debugging.
             * @param pc the parse context.
             * @return true if the child succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                return m_child->parse(pc);
            }

        private:
            parse_node_ptr_type m_child;
            std::string m_tag;
        };

        class rule;

        /**
         * A parse node that keeps a reference to a rule.
         */ 
        class rule_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param r the rule.
             */ 
            rule_parse_node(rule& r) : m_rule(r) {
            }

            /**
             * Returns the rule.
             * @return the rule.
             */ 
            rule& get_rule() const {
                return m_rule;
            }

            /**
             * It invokes the rule.
             * @param pc the parse context.
             * @return true if the rule succeeds, false otherwise.
             */ 
            bool parse(parse_context& pc) const override {
                return m_rule.parse(pc);
            }

        private:
            rule& m_rule;
        };

        /**
         * Holds a pointer to a parse node.
         * It is used for implementing the domain-specific language for this PEG parser.
         */ 
        class parse_node_ptr : public parse_node_ptr_type {
        public:
            /**
             * The default constructor.
             */ 
            parse_node_ptr() {
            }

            /**
             * Constructor from pointer.
             * @param node node pointer.
             */ 
            template <class T>
            parse_node_ptr(const std::shared_ptr<T>& node)
                : parse_node_ptr_type(node)
            {
            }

            /**
             * Constructor from symbol.
             * @param symbol symbol.
             */ 
            template <class Symbol, std::enable_if_t<!std::is_invocable_v<std::decay_t<Symbol>, parse_context&>, bool> = true>
            parse_node_ptr(Symbol symbol)
                : parse_node_ptr_type(std::make_shared<symbol_parse_node<Symbol>>(symbol))
            {
            }

            /**
             * Constructor from functor/function.
             * @param function the functor/function to invoke.
             */ 
            template <class F, std::enable_if_t<std::is_invocable_v<std::decay_t<F>, parse_context&>, bool> = true>
            parse_node_ptr(const F& function)
                : parse_node_ptr_type(std::make_shared<function_parse_node<F>>(function))
            {
            }

            /**
             * Constructor from basic string.
             * @param string the string.
             */ 
            template <class Char>
            parse_node_ptr(const std::basic_string<Char>& string)
                : parse_node_ptr_type(std::make_shared<string_parse_node<Char>>(string))
            {
            }

            /**
             * Constructor from null-terminated string.
             * @param string the string.
             */ 
            template <class Char>
            parse_node_ptr(const Char* string)
                : parse_node_ptr(std::basic_string<Char>(string))
            {
            }

            /**
             * Constructor from rule.
             * @param r the rule.
             */ 
            parse_node_ptr(rule& r)
                : parse_node_ptr_type(std::make_shared<rule_parse_node>(r))
            {
            }

            /**
             * Invokes the underlying parse node.
             * @param pc the parse context.
             * @return the result of the parse node invocation.
             */ 
            bool parse(parse_context& pc) const {
                return (*this)->parse(pc);
            }

            /**
             * Returns the name of the underlying parse node.
             * @return the name of the underlying parse node.
             */ 
            const std::string& get_name() const noexcept {
                return (*this)->get_name();
            }

            /**
             * Sets the name of the underlying parse node.
             * @param name the name of the underlying parse node.
             */ 
            void set_name(const std::string& name) {
                (*this)->set_name(name);
            }

            /**
             * Sets the name of the underlying parse node.
             * @param name the name of the underlying parse node.
             */ 
            parse_node_ptr& operator [](const std::string& name) {
                set_name(name);
                return *this;
            }
        };

        /**
         * A class that is used as a named placeholder for a parse node.
         * Since it is named, it can be used in recursive grammars.
         */ 
        class rule {
        public:
            /**
             * The default constructor.
             */ 
            rule() {
            }

            /**
             * Constructor from value.
             * @param value it can be a symbol, a string, a function, or a parse node.
             */ 
            template <class T>
            rule(T&& value) : m_node(parse_node_ptr(std::forward<T>(value))) {
            }

            /**
             * Invokes the underlying parse node.
             * @param pc the parse context.
             * @return the result of the parse node invocation.
             */ 
            bool parse(parse_context& pc) const {
                return m_node->parse(pc);
            }

            /**
             * Returns the name of the rule.
             * @return the name of the rule.
             */ 
            const std::string& get_name() const noexcept {
                return m_name;
            }

            /**
             * Sets the name of the rule.
             * @param name the name of the rule.
             */ 
            void set_name(const std::string& name) {
                m_name = name;
            }

        private:
            parse_node_ptr_type m_node;
            std::string m_name;
        };

        /**
         * Creates a terminal symbol parse node.
         * @param symbol the symbol.
         * @return the created parse node.
         */ 
        template <class Symbol>
        static parse_node_ptr term(Symbol symbol) {
            return symbol;
        }

        /**
         * Creates a terminal string parse node.
         * @param string the null-terminated string.
         * @return the created parse node.
         */ 
        template <class Char>
        static parse_node_ptr term(const Char* string) {
            return string;
        }

        /**
         * Creates a terminal set parse node.
         * @param string the null-terminated string.
         * @return the created parse node.
         */ 
        template <class Char>
        static parse_node_ptr set(const Char* string) {
            std::basic_string<Char> str{ string };
            std::vector<Char> vec{ str.begin(), str.end() };
            return std::make_shared<set_parse_node<Char>>(vec);
        }

        /**
         * Creates a terminal range parse node.
         * @param min the min value of the range.
         * @param max the max value of the range.
         * @return the created parse node.
         */ 
        template <class Symbol>
        static parse_node_ptr range(Symbol min, Symbol max) {
            return std::make_shared<range_parse_node<Symbol>>(min, max);
        }

        /**
         * Creates a 0 or more loop parse node.
         * @param node the node to convert to a loop.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator *(const parse_node_ptr& node) {
            return std::make_shared<loop_0_parse_node>(node);
        }

        /**
         * Creates a 1 or more loop parse node.
         * @param node the node to convert to a loop.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator +(const parse_node_ptr& node) {
            return std::make_shared<loop_1_parse_node>(node);
        }

        /**
         * Creates a loop parse node.
         * @param node the node to convert to a loop.
         * @param count number of times to repeat the node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator *(const parse_node_ptr& node, size_t count) {
            return std::make_shared<loop_n_parse_node>(node, count);
        }

        /**
         * Makes a parse node optional.
         * @param node the node to make optional of.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator -(const parse_node_ptr& node) {
            return std::make_shared<optional_parse_node>(node);
        }

        /**
         * Makes a logical-and predicate.
         * @param node the node to make a logical predicate of.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator &(const parse_node_ptr& node) {
            return std::make_shared<logical_and_parse_node>(node);
        }

        /**
         * Makes a logical-not predicate.
         * @param node the node to make a logical predicate of.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator !(const parse_node_ptr& node) {
            return std::make_shared<logical_not_parse_node>(node);
        }

        /**
         * Creates a sequence out of two parse nodes.
         * Any of the parse nodes that is a sequence is flattened into the result parse node.
         * param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator >>(const parse_node_ptr& left, const parse_node_ptr& right) {
            std::vector<std::shared_ptr<parse_node>> children;
            add_nodes<sequence_parse_node>(children, left);
            add_nodes<sequence_parse_node>(children, right);
            return std::make_shared<sequence_parse_node>(std::move(children));
        }

        /**
         * Creates an exclusion between two parse nodes.
         * It turns the expression `LEFT - RIGHT` to the expression `!RIGHT >> LEFT`,
         * essentially creating a barrier for `LEFT`.
         * param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator -(const parse_node_ptr& left, const parse_node_ptr& right) {
            return !right >> left;
        }

        /**
         * Creates a choice out of two parse nodes.
         * Any of the parse nodes that is a choice is flattened into the result parse node.
         * param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator |(const parse_node_ptr& left, const parse_node_ptr& right) {
            std::vector<std::shared_ptr<parse_node>> children;
            add_nodes<choice_parse_node>(children, left);
            add_nodes<choice_parse_node>(children, right);
            return std::make_shared<choice_parse_node>(std::move(children));
        }

        /**
         * Creates a match for a parse node.
         * @param node the parse node to match.
         * @param id the match id.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator ->*(const parse_node_ptr& node, MatchId id) {
            return std::make_shared<match_parse_node>(node, id);
        }

        /**
         * Creates a parse node that begins a match.
         * Subsequent matches start from the parse position established this created node.
         * @return the created parse node.
         */ 
        static parse_node_ptr begin_match() {
            return std::make_shared<begin_match_parse_node>();
        }

        /**
         * Creates a parse node that adds a match.
         * @param id the match id.
         * @return the created parse node.
         */ 
        static parse_node_ptr add_match(MatchId id) {
            return std::make_shared<add_match_parse_node>(id);
        }

        /**
         * Same as `add_match(id)`.
         */ 
        static parse_node_ptr match(MatchId id) {
            return add_match(id);
        }

        /**
         * Creates a match parse node that closes the current match.
         * @return the created parse node.
         */ 
        static parse_node_ptr end_match() {
            return std::make_shared<end_match_parse_node>();
        }

        /**
         * Creates a parse node that parses any symbol.
         * @return the created parse node.
         */ 
        static parse_node_ptr any() {
            return std::make_shared<any_parse_node>();
        }

        /**
         * Creates a parse node that matches the end of the source.
         * @return the created parse node.
         */ 
        static parse_node_ptr end() {
            return std::make_shared<end_parse_node>();
        }

        /**
         * Creates a parse node that makes a line-incrementing parse node.
         * @param node the node to use for parsing a newline.
         * @return the created parse node.
         */ 
        static parse_node_ptr newline(const parse_node_ptr& node) {
            return std::make_shared<newline_parse_node>(node);
        }

        /**
         * Creates a function parse node.
         * @param function the functor/function to invoke.
         * @return the created parse node.
         */ 
        template <class F>
        static parse_node_ptr function(const F& function) {
            return std::make_shared<function_parse_node<F>>(function);
        }

        /**
         * Creates a parse node that adds an error to a parse context.
         * @param skip_node the node to use to skip to the end of the erroneous input.
         * @param id the error id.
         * @return the created parse node.
         */ 
        static parse_node_ptr error(const parse_node_ptr& skip_node, ErrorId id) {
            return std::make_shared<error_parse_node>(skip_node, id);
        }

        /**
         * Creates a skip parse node that skips to just before the recognized input.
         * @param id the error id.
         * @return the created parse node.
         */ 
        static parse_node_ptr skip_before(const parse_node_ptr& node) {
            return std::make_shared<skip_before_parse_node>(node);
        }

        /**
         * Creates a skip parse node that skips to just after the recognized input.
         * @param id the error id.
         * @return the created parse node.
         */ 
        static parse_node_ptr skip_after(const parse_node_ptr& node) {
            return std::make_shared<skip_after_parse_node>(node);
        }

        /**
         * Creates a debug parse node.
         * @param node the node to debug.
         * @param tag the debug tag.
         * @return the created parse node.
         */ 
        static parse_node_ptr debug(const parse_node_ptr& node, const std::string& tag = {}) {
            return std::make_shared<debug_parse_node>(node, tag);
        }

        /**
         * It creates the following sequence:
         * 
         * `BEGIN_MATCH >> BASE >> *(... | BRANCHES) >> END_MATCH`.
         * 
         * It is used for handling left-associative parsing.
         * 
         * The `BEGIN_MATCH` parse node establishes a match start position.
         * 
         * The branches can then use the function `match(id)` to repeateadly add matches
         * that start from the above match start position.
         * 
         * Finally, the `END_MATCH` parse node removes the established match start position.
         * 
         * When a match happens, the child matches for a parent match start from the established match start position,
         * effectively allowing for left-associative parsing.
         * 
         * Example:
         * 
         * @code
         * //
         * // Assuming the EBNF left-associative grammar is:
         * // add = add '+' mul
         * //     | add '-' mul
         * //     | mul
         * //     ;
         * //
         * auto add = my_parser::left_associative(
         *      mul, 
         *      '+' >> mul >> my_parser::match(ADD), 
         *      '-' >> mul >> my_parser::match(SUB)
         *  );
         * @endcode
         * 
         * @param base the parse node that starts the left-associative expression.
         * @param branches the repeated branches.
         * @return the created parse node.
         */ 
        template <class... T>
        static parse_node_ptr left_associative(const parse_node_ptr& base, const T&... branches) {
            return begin_match() >> base >> *(... | parse_node_ptr(branches)) >> end_match();
        }

    private:
        template <class T>
        static void add_nodes(std::vector<std::shared_ptr<parse_node>>& nodes, const std::shared_ptr<parse_node>& child) {
            const T* parent = dynamic_cast<const T*>(child.get());
            if (parent) {
                nodes.insert(nodes.end(), parent->get_children().begin(), parent->get_children().end());
            }
            else {
                nodes.push_back(child);
            }
        }
    }; //class parser


} //namespace parserlib


#endif //PARSERLIB_HPP
