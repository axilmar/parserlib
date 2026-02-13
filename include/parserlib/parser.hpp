#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <cassert>
#include <memory>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <type_traits>
#include <initializer_list>
#include "parse_context.hpp"
#include "vector.hpp"


namespace parserlib {


    template <class ParseContext = parse_context<>> class parser {
    public:
        class false_parse_node;
        class true_parse_node;
        class rule;

        /**
         * Base class for parse nodes.
         */ 
        class parse_node {
        public:
            /**
             * The destructor.
             */ 
            virtual ~parse_node() {
            }

            /**
             * Interface for parsing.
             * Subclasses provide their own parsing algorithm.
             * @param pc the parse context to use for parsing.
             * @return true on success, false on failure.
             */ 
            virtual bool parse(ParseContext& pc) const = 0;
        };

        /**
         * Pointer to parse node.
         */ 
        class parse_node_ptr : public std::shared_ptr<parse_node> {
        public:
            using std::shared_ptr<parse_node>::shared_ptr;

            /**
             * Constructor from symbol.
             * It creates a symbol parse node.
             * @param symbol the symbol to parse.
             */ 
            template <class T> parse_node_ptr(const T& symbol) 
                : std::shared_ptr<parse_node>(std::make_shared<symbol_parse_node<T>>(symbol))
            {
            }

            /**
             * Constructor from string.
             * It creates a string parse node.
             * @param string the string to parse.
             */ 
            template <class T> parse_node_ptr(const T* string) 
                : std::shared_ptr<parse_node>(std::make_shared<string_parse_node<T>>(string))
            {
            }

            /**
             * Constructor from rule.
             * Since rules are allowed to be recursive, a reference parse node is created for the given rule.
             * References for rules are updated each time the rule is set.
             * @param r the rule to create a reference parse node.
             */ 
            parse_node_ptr(const rule& r) 
                : std::shared_ptr<parse_node>(r.get_ref_node())
            {
            }

            /**
             * Constructor from a boolean value.
             * It creates either a true or a false parse node.
             * @param b the boolean value to create a boolean parse node for.
             */ 
            parse_node_ptr(bool b)
                : std::shared_ptr<parse_node>(create_bool_parse_node(b))
            {
            }

            /**
             * Operator that creates a 0-or-more times loop parse node out of this parse node.
             * @return a 0-or-more times loop parse node out of this parse node.
             */ 
            parse_node_ptr operator *() const {
                return std::make_shared<loop_0_parse_node>(*this);
            }

            /**
             * Operator that creates a 1-or-more times loop parse node out of this parse node.
             * @return a 1-or-more times loop parse node out of this parse node.
             */ 
            parse_node_ptr operator +() const {
                return std::make_shared<loop_1_parse_node>(*this);
            }

            /**
             * Operator that creates an optional parse node out of this parse node.
             * @return an optional parse node out of this parse node.
             */ 
            parse_node_ptr operator -() const {
                return std::make_shared<optional_parse_node>(*this);
            }

            /**
             * Operator that creates a logical and parse node out of this parse node.
             * @return a logical and parse node out of this parse node.
             */ 
            parse_node_ptr operator &() const {
                return std::make_shared<logical_and_parse_node>(*this);
            }

            /**
             * Operator that creates a logical not parse node out of this parse node.
             * @return a logical not parse node out of this parse node.
             */ 
            parse_node_ptr operator !() const {
                return std::make_shared<logical_not_parse_node>(*this);
            }

            /**
             * Invokes the underlying parse node with the given parse context.
             * @param pc the parse context to pass to the underlying parse node.
             * @return the result of the parse node.
             */ 
            bool parse(ParseContext& pc) const {
                return (*this)->parse(pc);
            }

        private:
            static std::shared_ptr<parse_node> create_bool_parse_node(bool b) {
                return b ? 
                    std::static_pointer_cast<parse_node>(std::make_shared<true_parse_node>()) : 
                    std::static_pointer_cast<parse_node>(std::make_shared<false_parse_node>());
            }
        };

        /**
         * A container of parse nodes (via ptr).
         */ 
        using parse_node_vector = std::vector<parse_node_ptr>;


        /**
         * Parent class with one child.
         */ 
        class parent_parse_node_single_child : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child.
             */ 
            parent_parse_node_single_child(const parse_node_ptr& child) : m_child(child) {
            }

            /**
             * Returns the one and only child.
             * @return the one and only child.
             */ 
            const parse_node_ptr& get_child() const {
                return m_child;
            }

        private:
            const parse_node_ptr m_child;
        };

        /**
         * Parent class with multiple children.
         */ 
        class parent_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param children the children.
             */ 
            parent_parse_node(const parse_node_vector& children) : m_children(children) {
            }

            /**
             * Returns the children.
             * @return the children.
             */ 
            const parse_node_vector& get_children() const {
                return m_children;
            }

        private:
            const parse_node_vector m_children;
        };

        /**
         * A parse node that parses a single value.
         * @param T the type of value to parse; must be convertible to 'int'.
         */ 
        template <class T> class symbol_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param symbol the symbol to parse.
             */ 
            symbol_parse_node(const T& symbol) : m_symbol(symbol) {
            }

            /**
             * Parses the given symbol.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    if (ParseContext::compare(token, m_symbol) == 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            const T m_symbol;
        };

        /**
         * A parse node that parses a string.
         * @param T the type of character.
         */ 
        template <class T> class string_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param string the string to parse.
             */ 
            string_parse_node(const std::basic_string_view<T>& string) : m_string(string) {
            }

            /**
             * Parses the given string.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    auto itStr = m_string.begin();
                    auto itSrc = pc.get_iterator();
                    for (;;) {
                        if (itStr == m_string.end()) {
                            pc.increment_iterator(m_string.size());
                            return true;
                        }
                        if (itSrc == pc.get_end_iterator()) {
                            break;
                        }
                        const auto& strch = *itStr;
                        const auto& token = *itSrc;
                        if (ParseContext::compare(token, strch) != 0) {
                            break;
                        }
                        ++itStr;
                        ++itSrc;
                    }
                }
                return false;
            }

        private:
            const std::basic_string<T> m_string;
        };

        /**
         * A parse node that parses a single value out of set of values.
         * @param T the type of value.
         */ 
        template <class T> class set_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param set the set of values to parse; a sorted version of this is kept internally.
             */ 
            set_parse_node(const std::vector<T>& set) : m_set(get_sorted_vector(set)) {
            }

            /**
             * Parses a single value out of the set.
             * It uses the algorithm `std::upper_bound`, for increased efficiency.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), token, [](const auto& a, const auto& b) {
                        return ParseContext::compare(a, b) < 0;
                    });
                    if (it != m_set.begin()) {
                        --it;
                        if (ParseContext::compare(token, *it) == 0) {
                            pc.increment_iterator();
                            return true;
                        }
                    }
                }
                return false;
            }

        private:
            const std::vector<T> m_set;

            static std::vector<T> get_sorted_vector(const std::vector<T>& set) {
                std::vector<T> result(set.begin(), set.end());
                std::sort(result.begin(), result.end());
                return result;
            }
        };

        /**
         * A parse node that parses a value out of a range.
         * @param T type of value of the range.
         */ 
        template <class T> class range_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param min the min value.
             * @param max the max value.
             */ 
            range_parse_node(const T& min, const T& max) : m_min(min), m_max(max) {
                assert(min <= max);
            }

            /**
             * Parses a value out of the range of values.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    if (ParseContext::compare(token, m_min) >= 0 && ParseContext::compare(token, m_max) <= 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            const T m_min;
            const T m_max;
        };

        /**
         * A parse node that repeats another node 0 or more times. 
         */ 
        class loop_0_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * Repeats the child node as many times as the child node returns true,
             * and while the parse iterator is incremented.
             * @param pc the parse context to use.
             * @return always true.
             */ 
            bool parse(ParseContext& pc) const override {
                for (;;) {
                    const auto state = pc.get_state();
                    try {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc) || pc.get_iterator() == state.get_iterator()) {
                            pc.set_state(state);
                            break;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                }
                return true;
            }
        };

        /**
         * A parse node that repeats another node 1 or more times. 
         */ 
        class loop_1_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * Invokes the child node once. If that is successful,
             * then it repeats the child node as many times as the child node returns true,
             * and while the parse iterator is incremented.
             * @param pc the parse context to use.
             * @return true if the first loop was successful, false if the first loop was unsuccessful.
             *  If the first loop is successful, then all subsequent loops are considered successful
             *  and true is returned.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    if (!child->parse(pc)) {
                        pc.set_state(state);
                        return false;
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }

                for (;;) {
                    const auto state = pc.get_state();
                    try {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc) || pc.get_iterator() == state.get_iterator()) {
                            pc.set_state(state);
                            break;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                }

                return true;
            }
        };

        /**
         * A parse node that repeats another node a specific amount of times. 
         */ 
        class loop_n_parse_node : public parent_parse_node_single_child {
        public:
            /**
             * The constructor.
             * @param count the number of times the child node must be repeated.
             * @param child the child node.
             */ 
            loop_n_parse_node(size_t count, const parse_node_ptr& child)
                : parent_parse_node_single_child(child)
                , m_count(count)
            {
            }

            /**
             * It repeats the child node the given amount of times.
             * The parsing is considered successful if the child node parses successfully
             * for all the specified times.
             * @param pc the parse context to use.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    for (size_t i = 0; i < m_count; ++i) {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc)) {
                            pc.set_state(state);
                            return false;
                        }
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
                return true;
            }

        private:
            const size_t m_count;
        };

        /**
         * A parse node that makes another node optional.
         */ 
        class optional_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * It invokes the child node and then discards the result, returning true.
             * @param pc the parse context to use.
             * @return always true.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    if (!child->parse(pc)) {
                        pc.set_state(state);
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
                return true;
            }
        };

        /**
         * A parse node that uses another parse node for testing the input.
         */ 
        class logical_and_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * It invokes the parse node, then undoes any side effects (matches, errors, etc)
             * done by the child node and returns the result of the child node.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    const bool result = child->parse(pc);
                    pc.set_state(state);
                    return result;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        /**
         * A parse node that uses another parse node for testing the input,
         * inverting the result of that other node.
         */ 
        class logical_not_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * It invokes the parse node, then undoes any side effects (matches, errors, etc)
             * done by the child node and returns the opposite result of the child node.
             * @param pc the parse context.
             * @return true on success (i.e. the child returned false), false on failure (i.e. the child returned true).
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    const bool result = !child->parse(pc);
                    pc.set_state(state);
                    return result;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        /**
         * A parse node that parses a group of other nodes in the order they were added in the group.
         * It stops when a child returns false.
         * If it fails, the parse context state is set to the state before this is invoked.
         */ 
        class sequence_parse_node : public parent_parse_node {
        public:
            using parent_parse_node::parent_parse_node;

            /**
             * Invokes the children, in the order they are added to the group, stopping when a child returns false.
             * @param pc the parse context.
             * @return true if all children parsed successfully, false if at least one child failed to parse.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    for (const parse_node_ptr& child : this->get_children()) {
                        if (!child->parse(pc)) {
                            pc.set_state(state);
                            return false;
                        }
                    }
                    return true;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        /**
         * A parse node that parses a group of other nodes in the order they were added in the group.
         * It stops when a child returns true.
         * If it fails, the parse context state is set to the state before this is invoked.
         */ 
        class choice_parse_node : public parent_parse_node {
        public:
            using parent_parse_node::parent_parse_node;

            /**
             * Invokes the children, in the order they are added to the group, stopping when a child returns true.
             * @param pc the parse context.
             * @return true if one child parsed successfully, false if all children failed to parse.
             */ 
            bool parse(ParseContext& pc) const override {
                for (const parse_node_ptr& child : this->get_children()) {
                    const auto state = pc.get_state();
                    try {
                        if (child->parse(pc)) {
                            return true;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                    pc.set_state(state);
                }
                return false;
            }
        };

        /**
         * A parse node that adds a match to the parse context
         * if its child node parses successfully.
         */ 
        class match_parse_node : public parent_parse_node_single_child {
        public:
            /**
             * The constructor.
             * @param id the match id.
             * @param child the child node.
             */ 
            match_parse_node(typename ParseContext::match_id_type id, const parse_node_ptr& child) 
                : parent_parse_node_single_child(child), m_id(id)
            {
            }

            /**
             * Invokes the child node.
             * If the child node succeeds, a match is added to the parse context.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                const auto match_start_state = pc.get_match_start_state();
                const parse_node_ptr& child = this->get_child();
                if (child->parse(pc)) {
                    pc.add_match(m_id, match_start_state);
                    return true;
                }
                return false;
            }

        private:
            typename ParseContext::match_id_type m_id;
        };

        /**
         * A parse node that parses any symbol.
         */ 
        class any_parse_node : public parse_node {
        public:
            /**
             * Parses any symbol.
             * @param pc the parse context.
             * @return true if the end of source was not reached, false if the end of source was reached.
             */ 
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    pc.increment_iterator();
                    return true;
                }
                return false;
            }
        };

        /**
         * A parse node that checks if the end of source is reached.
         */ 
        class end_parse_node : public parse_node {
        public:
            /**
             * Checks if the end of source is reached.
             * @param pc the parse context.
             * @return true if the end is reached, false otherwise.
             */ 
            bool parse(ParseContext& pc) const override {
                return pc.is_end_iterator();
            }
        };

        /**
         * A parse node that returns false.
         */ 
        class false_parse_node : public parse_node {
        public:
            /**
             * Returns false.
             * @param pc the parse context.
             * @return always false.
             */ 
            bool parse(ParseContext& pc) const override {
                return false;
            }
        };

        /**
         * A parse node that returns true.
         */ 
        class true_parse_node : public parse_node {
        public:
            /**
             * Returns true.
             * @param pc the parse context.
             * @return always true.
             */ 
            bool parse(ParseContext& pc) const override {
                return true;
            }
        };

        /**
         * A parse node that increments the parse context current line
         * if the child parse node parses successfully.
         * The parse context's iterator must have the method `increment_line()`.
         */ 
        class newline_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            /**
             * Invokes the child node, then increments the line if the child node returns true.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                const parse_node_ptr& child = this->get_child();
                if (child->parse(pc)) {
                    pc.increment_line();
                    return true;
                }
                return false;
            }
        };

        /**
         * Invokes a function for parsing.
         * @param T type of the function.
         */ 
        template <class T> class function_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param function the function to invoke.
             */ 
            function_parse_node(const T& function) : m_function(function) {
            }

            /**
             * It invokes the function.
             * @param pc the parse context.
             * @return what the function reutrns.
             */ 
            bool parse(ParseContext& pc) const override {
                return m_function(pc);
            }

        private:
            const T m_function;
        };

        /**
         * A reference parse node.
         * It is used to create loops.
         * Internally, it holds a weak pointer to a parse node,
         * to avoid referential cycles.
         */ 
        class ref_parse_node : public parse_node {
        public:
            /**
             * The constructor.
             * @param child the child node.
             */ 
            ref_parse_node(const parse_node_ptr& child) : m_child(child) {
            }

            /**
             * Sets the child node.
             * @param child the child node.
             */ 
            void set_child(const parse_node_ptr& child) {
                m_child = child;
            }

            /**
             * It invokes the child node.
             * @param pc the parse context.
             * @return true on success, false on failure.
             */ 
            bool parse(ParseContext& pc) const override {
                const std::shared_ptr<parse_node> child = m_child.lock();
                return child->parse(pc);
            }

        private:
            std::weak_ptr<parse_node> m_child;
        };

        /**
         * A class that represents a named parse node.
         * Internally, it has a parse_node_ptr instance.
         * It can be used to create a recursive grammar.
         * Rules handle left recursion if one is found.
         */ 
        class rule {
        public:
            /**
             * The default constructor.
             * It creates an empty rule.
             */ 
            rule() {
                set_constructed();
            }

            /**
             * The copy constructor.
             * Any references to this rule prior to the invocation of this constructor are updated
             * with the specified value.
             * @param r the source rule.
             */ 
            rule(const rule& r) : m_node(r.m_node) {
                set_constructed();
                update_refs();
            }

            rule(rule&&) = delete;

            /**
             * The constructor from a parse node ptr.
             * Any references to this rule prior to the invocation of this constructor are updated
             * with the specified value.
             * @param r the source rule.
             */ 
            rule(const parse_node_ptr& node) : m_node(node) {
                set_constructed();
                update_refs();
            }

            /**
             * The destructor.
             * After this call, the references to this parse node cannot be updated.
             */ 
            ~rule() {
                reset_constructed();
                remove_refs();
            }

            /**
             * The copy assignment operator.
             * Any references to this rule prior to the invocation of this constructor are updated
             * with the specified value.
             * @param r the source rule.
             */ 
            rule& operator = (const rule& r) {
                m_node = r.m_node;
                update_refs();
                return *this;
            }

            /**
             * The copy assignment operator from a parse node ptr.
             * Any references to this rule prior to the invocation of this constructor are updated
             * with the specified value.
             * @param r the source rule.
             */ 
            rule& operator = (const parse_node_ptr& node) {
                m_node = node;
                update_refs();
                return *this;
            }

            /**
             * Returns a reference node for this rule.
             * @return a reference node for this rule.
             */ 
            parse_node_ptr get_ref_node() const {
                std::shared_ptr<ref_parse_node> ref_node = std::make_shared<ref_parse_node>(is_constructed() ? m_node : nullptr);
                get_rule_ref_map()[this].push_back(ref_node);
                return ref_node;
            }

            /**
             * Operator that creates a 0-or-more times loop parse node out of the parse node of this rule.
             * @return a 0-or-more times loop parse node out of the parse node of this rule.
             */ 
            parse_node_ptr operator *() const {
                return std::make_shared<loop_0_parse_node>(get_ref_node());
            }

            /**
             * Operator that creates a 1-or-more times loop parse node out of the parse node of this rule.
             * @return a 1-or-more times loop parse node out of the parse node of this rule.
             */ 
            parse_node_ptr operator +() const {
                return std::make_shared<loop_1_parse_node>(get_ref_node());
            }

            /**
             * Operator that creates an optional parse node out of the parse node of this rule.
             * @return an optional parse node out of the parse node of this rule.
             */ 
            parse_node_ptr operator -() const {
                return std::make_shared<optional_parse_node>(get_ref_node());
            }

            /**
             * Operator that creates a logical and parse node out of the parse node of this rule.
             * @return a logical and parse node out of the parse node of this rule.
             */ 
            parse_node_ptr operator &() const {
                return std::make_shared<logical_and_parse_node>(get_ref_node());
            }

            /**
             * Operator that creates a logical not parse node out of the parse node of this rule.
             * @return a logical not parse node out of the parse node of this rule.
             */ 
            parse_node_ptr operator !() const {
                return std::make_shared<logical_not_parse_node>(get_ref_node());
            }

            /**
             * Invokes the internal parse node.
             * @param pc the parse context.
             * @return what the internal parse node returns.
             */ 
            bool parse(ParseContext& pc) const {
                return m_node->parse(pc);
            }

        private:
            //constructed rule map
            using rule_set = std::set<const rule*>;

            //set of references per rule
            using rule_ref_map = std::map<const rule*, std::vector<std::shared_ptr<ref_parse_node>>>;
            
            //the internal node
            parse_node_ptr m_node;

            //returns the constructed rule map
            static rule_set& get_rule_set() {
                static rule_set set;
                return set;
            }

            //returns the references per rule map
            static rule_ref_map& get_rule_ref_map() {
                static rule_ref_map map;
                return map;
            }

            //check if the rule is constructed; 
            //if the rule is not constructed, do not access its internal node ptr
            bool is_constructed() const {
                return get_rule_set().find(this) != get_rule_set().end();
            }

            //sets the rule as constructed
            void set_constructed() const {
                get_rule_set().insert(this);
            }

            //resets the rule from being constructed
            void reset_constructed() const {
                get_rule_set().erase(this);
            }

            //updates the references of the rule
            void update_refs() const {
                const auto it = get_rule_ref_map().find(this);
                if (it != get_rule_ref_map().end()) {
                    for (const auto& ref : it->second) {
                        ref->set_child(m_node);
                    }
                }
            }

            //removes any references for the rule
            void remove_refs() const {
                get_rule_ref_map().erase(this);
            }
        };

        /**
         * Creates a symbol parse node for the given symbol.
         * @param symbol the symbol to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr terminal(const T& symbol) {
            return std::make_shared<symbol_parse_node<T>>(symbol);
        }

        /**
         * Creates a string parse node for the given string.
         * @param string the string to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr terminal(const T* string) {
            return std::make_shared<string_parse_node<T>>(string);
        }

        /**
         * Creates a set parse node for the given string.
         * @param string the string to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr set(const T* set) {
            std::basic_string_view<T> string(set);
            return std::make_shared<set_parse_node<T>>(std::vector<T>(string.begin(), string.end()));
        }

        /**
         * Creates a set parse node for the given list of symbols.
         * @param string the string to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr set(const std::initializer_list<T>& set) {
            return std::make_shared<set_parse_node<T>>(std::vector<T>(set.begin(), set.end()));
        }

        /**
         * Creates a range parse node.
         * @param min min value.
         * @param max max value.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr range(const T& min, const T& max) {
            return std::make_shared<range_parse_node<T>>(min, max);
        }

        /**
         * Creates an any parse node.
         * @return pointer to the created parse node.
         */ 
        static parse_node_ptr any() {
            static parse_node_ptr node = std::make_shared<any_parse_node>();
            return node;
        }

        /**
         * Creates an end parse node.
         * @return pointer to the created parse node.
         */ 
        static parse_node_ptr end() {
            static parse_node_ptr node = std::make_shared<end_parse_node>();
            return node;
        }

        /**
         * Creates a false parse node.
         * @return pointer to the created parse node.
         */ 
        static parse_node_ptr false_() {
            static parse_node_ptr node = std::make_shared<false_parse_node>();
            return node;
        }

        /**
         * Creates a true parse node.
         * @return pointer to the created parse node.
         */ 
        static parse_node_ptr true_() {
            static parse_node_ptr node = std::make_shared<true_parse_node>();
            return node;
        }

        /**
         * Creates a newline parse node.
         * @param child the child parse node; usually a terminal for characters '\n', '\r' or a combination of.
         * @return pointer to the created parse node.
         */ 
        static parse_node_ptr newline(const parse_node_ptr& child) {
            return std::make_shared<newline_parse_node>(child);
        }

        /**
         * Creates a parse node for a function.
         * @param function the function to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr function(const T& function) {
            return std::make_shared<function_parse_node<T>>(function);
        }

        /**
         * An alternative way to create a parse node.
         * @param val the value to create a parse node for.
         * @return pointer to the created parse node.
         */ 
        template <class T> static parse_node_ptr make_parse_node(const T& val) {
            return val;
        }

        /**
         * Creates a sequence out of two parse nodes.
         * Sequences are flattened.
         * @param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator >> (const parse_node_ptr& left, const parse_node_ptr& right) {
            const sequence_parse_node* left_sequence = dynamic_cast<const sequence_parse_node*>(left.get());
            const sequence_parse_node* right_sequence = dynamic_cast<const sequence_parse_node*>(right.get());
            if (left_sequence && right_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left_sequence->get_children(), right_sequence->get_children()));
            }
            else if (left_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left_sequence->get_children(), right));
            }
            else if (right_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left, right_sequence->get_children()));
            }
            else {
                return std::make_shared<sequence_parse_node>(vector_cat(left, right));
            }
        }

        /**
         * Creates a choice out of two parse nodes.
         * Choices are flattened.
         * @param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator | (const parse_node_ptr& left, const parse_node_ptr& right) {
            const choice_parse_node* left_choice = dynamic_cast<const choice_parse_node*>(left.get());
            const choice_parse_node* right_choice = dynamic_cast<const choice_parse_node*>(right.get());
            if (left_choice && right_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left_choice->get_children(), right_choice->get_children()));
            }
            else if (left_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left_choice->get_children(), right));
            }
            else if (right_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left, right_choice->get_children()));
            }
            else {
                return std::make_shared<choice_parse_node>(vector_cat(left, right));
            }
        }

        /**
         * Creates an exclusion expression.
         * It converts the expression `A - B` to `!B >> A`.
         * @param left the left parse node.
         * @param right the right parse node.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator - (const parse_node_ptr& left, const parse_node_ptr& right) {
            return !right >> left;
        }

        /**
         * Creates a parse node that repeats another parse node an explicit amount of times.
         * @param count number of times to repeat the given parse node.
         * @param node the node to repeat.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator * (size_t count, const parse_node_ptr& node) {
            return std::make_shared<loop_n_parse_node>(count, node);
        }

        /**
         * Creates a match parse node.
         * @param node the node to create a match for.
         * @param id the id of the match.
         * @return the created parse node.
         */ 
        friend parse_node_ptr operator ->* (const parse_node_ptr& node, typename ParseContext::match_id_type id) {
            return std::make_shared<match_parse_node>(id, node);
        }

    }; //class parser


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
