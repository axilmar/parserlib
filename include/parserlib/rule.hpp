#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <stdexcept>
#include "parse_context.hpp"
#include "loop_0_parse_node.hpp"
#include "loop_1_parse_node.hpp"
#include "optional_parse_node.hpp"
#include "logical_and_parse_node.hpp"
#include "logical_not_parse_node.hpp"
#include "rule_ref_parse_node.hpp"


namespace parserlib {


    /**
     * A rule represents a piece of grammar that can be recursive.
     * Rules in grammar trees are manipulated via references.
     * @param ParseContext the parse context to use for this rule.
     */
    template <class ParseContext = parse_context<>>
    class rule {
    public:
        /** Type of rule reference parse node. */
        using rule_ref_type = rule_ref_parse_node<ParseContext>;

        /**
         * Constructs an empty rule.
         */
        rule() {
        }

        /**
         * The copy constructor.
         * Deleted because rules are managed via references.
         */
        rule(const rule&) = delete;

        /**
         * The move constructor.
         * Deleted because rules are managed via references.
         */
        rule(rule&&) = delete;

        /**
         * Constructor from a parse node or expression.
         * @param parse_node the parse node or expression to store internally.
         */
        template <class ParseNode> 
        rule(const ParseNode& parse_node)
            : m_parse_node(create_wrapper(parse_node))
        {
        }

        /**
         * Constructor from rule reference.
         * @param r rule to create a rule reference to.
         */
        rule(rule& r)
            : m_parse_node(create_wrapper(r))
        {
        }

        /**
         * The copy assignment operator.
         * Deleted because rules are managed via references.
         */
        rule& operator = (const rule&) = delete;

        /**
         * The move assignment operator.
         * Deleted because rules are managed via references.
         */
        rule& operator = (rule&&) = delete;

        /**
         * Assignment from parse node or expression.
         * @param parse_node the parse node or expression to store internally.
         * @return reference to this.
         */
        template <class ParseNode>
        rule& operator = (const ParseNode& parse_node) {
            m_parse_node = create_wrapper(parse_node);
            return *this;
        }

        /**
         * Assignment from rule; it creates a rule reference to the given rule.
         * @param r rule to create a rule reference to.
         * @return reference to this.
         */
        rule& operator = (rule& r) {
            m_parse_node = create_wrapper(r);
            return *this;
        }

        /**
         * Creates a loop of 0 or more times for this rule.
         * @return a loop of 0 or more times for this rule.
         */
        loop_0_parse_node<rule_ref_type> operator *() {
            return rule_ref_type(*this);
        }

        /**
         * Creates a loop of 1 or more times for this rule.
         * @return a loop of 1 or more times for this rule.
         */
        loop_1_parse_node<rule_ref_type> operator +() {
            return rule_ref_type(*this);
        }

        /**
         * Creates an optional parse node for this rule.
         * @return an optional parse node for this rule.
         */
        optional_parse_node<rule_ref_type> operator -() {
            return rule_ref_type(*this);
        }

        /**
         * Creates a logical and parse node for this rule.
         * @return a logical and parse node for this rule.
         */
        logical_and_parse_node<rule_ref_type> operator &() {
            return rule_ref_type(*this);
        }

        /**
         * Creates a logical not parse node for this rule.
         * @return a logical not parse node for this rule.
         */
        logical_not_parse_node<rule_ref_type> operator !() {
            return rule_ref_type(*this);
        }

        /**
         * Returns a pointer to this rule, since operator & is taken for `logical AND operator`.
         * @return a pointer to this rule.
         */
        rule* this_() {
            return this;
        }

        /**
         * Uses the internal expression in order to handle parsing.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) {
            return m_parse_node->parse(pc);
        }

    private:
        //interface
        class _inter {
        public:
            virtual ~_inter() {}
            virtual bool parse(ParseContext&) const = 0;
        };

        //implementation
        template <class T> class _impl : public _inter {
        public:
            _impl(const T& parse_node) : m_parse_node(parse_node) {}
            bool parse(ParseContext& pc) const override { return m_parse_node.parse(pc); }
        private:
            const T m_parse_node;
        };

        //the parse node ptr
        std::unique_ptr<_inter> m_parse_node;

        //create a wrapper for a parse node
        template <class ParseNode>
        static std::unique_ptr<_inter> create_wrapper(const ParseNode& parse_node) {
            return std::make_unique<_impl<decltype(make_parse_node(parse_node))>>(make_parse_node(parse_node));
        }

        //create a wrapper for a rule
        static std::unique_ptr<_inter> create_wrapper(rule& r) {
            return std::make_unique<_impl<decltype(make_parse_node(r))>>(make_parse_node(r));
        }
    };


    /**
     * Creates a parse node for a rule.
     * Rules are always managed by reference, in order to implement recursion.
     * @param r the rule to use for parsing.
     * @return a rule reference.
     */
    template <class ParseContext>
    rule_ref_parse_node<ParseContext> make_parse_node(rule<ParseContext>& r) {
        return r;
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
