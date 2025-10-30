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

        /** This class' type. */
        using rule_type = rule<ParseContext>;

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
        rule* this_ptr() {
            return this;
        }

        /**
         * Uses the internal expression in order to handle parsing.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) {
            const auto it = pc.m_rule_data.find(this);

            //initial entrance
            if (it == pc.m_rule_data.end()) {
                auto [it1, ok] = pc.m_rule_data.insert(std::make_pair(this, rule_data_type{ pc.parse_position().iterator(), ParseContext::rule_state::none }));
                try {
                    return m_parse_node->parse(pc);
                }
                catch (left_recursion ex) {
                    if (ex.rule == this) {
                        return parse_left_recursion(pc, it1->second);
                    }
                    else {
                        throw ex;
                    }
                }
            }

            //subsequent entrance; start new state if iterator has advanced from last time
            else if (it->second.iterator != pc.parse_position().iterator()) {
                const auto prev_state = it->second;
                it->second.iterator = pc.parse_position().iterator();
                it->second.state = ParseContext::rule_state::none;
                try {
                    const bool result = m_parse_node->parse(pc);
                    it->second = prev_state;
                    return result;
                }
                catch (left_recursion ex) {
                    if (ex.rule == this) {
                        try {
                            const bool result = parse_left_recursion(pc, it->second);
                            it->second = prev_state;
                            return result;
                        }
                        catch (...) {
                            it->second = prev_state;
                            throw;
                        }
                    }
                    else {
                        it->second = prev_state;
                        throw ex;
                    }
                }
            }

            //left recursion found
            else {
                switch (it->second.state) {
                    case ParseContext::rule_state::none:
                        throw left_recursion{ this };

                    case ParseContext::rule_state::reject:
                        return false;

                    case ParseContext::rule_state::accept:
                        return true;
                }
            }

            throw std::logic_error("rule: parse: invalid state");
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

        //rule data type
        using rule_data_type = typename ParseContext::rule_data;

        //used for initiating a left recursion parsing
        struct left_recursion {
            rule_type* rule;
        };

        //parse left recursion
        bool parse_left_recursion(ParseContext& pc, rule_data_type& rd) {
            //reject
            rd.state = ParseContext::rule_state::reject;
            if (!m_parse_node->parse(pc)) {
                return false;
            }

            const auto prev_lrpos = pc.m_left_recursion_parse_position;

            //accept
            rd.state = ParseContext::rule_state::accept;
            for(;;) {
                pc.m_left_recursion_parse_position = pc.parse_position();
                try {
                    if (!m_parse_node->parse(pc)) {
                        break;
                    }
                }
                catch (...) {
                    pc.m_left_recursion_parse_position = prev_lrpos;
                    throw;
                }
            }

            //success
            pc.m_left_recursion_parse_position = prev_lrpos;
            return true;
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
