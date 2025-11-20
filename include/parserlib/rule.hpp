#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <stdexcept>
#include <sstream>
#include "parse_context.hpp"
#include "loop_0_parse_node.hpp"
#include "loop_1_parse_node.hpp"
#include "optional_parse_node.hpp"
#include "logical_and_parse_node.hpp"
#include "logical_not_parse_node.hpp"
#include "rule_ref_parse_node.hpp"
#include "annotation_parse_node.hpp"
#include "named_parse_node.hpp"
#include "rule_optimizations.hpp"


namespace parserlib {


    /**
     * A rule represents a piece of grammar that can be recursive.
     * Rules in grammar trees are manipulated via references.
     * @param ParseContext the parse context to use for this rule.
     */
    template <class ParseContext = parse_context<>>
    class rule : public parse_node_base {
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
         * Constructor from a named parse ode.
         * @param parse_node the parse node or expression to store internally.
         */
        #ifndef NDEBUG
        template <class ParseNode> 
        rule(const named_parse_node<ParseNode>& parse_node)
            : m_parse_node(create_wrapper(parse_node.parse_node()))
            , m_name(parse_node.name())
        {
        }
        #endif

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
         * Assignment from named parse node.
         * @param parse_node the parse node or expression to store internally.
         * @return reference to this.
         */
        #ifndef NDEBUG
        template <class ParseNode>
        rule& operator = (const named_parse_node<ParseNode>& parse_node) {
            m_parse_node = create_wrapper(parse_node.parse_node());
            m_name = parse_node.name();
            return *this;
        }
        #endif

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
         * Returns a node that adds an annotation to this rule.
         * @param annotation the annotation object.
         * @return an annotation parse node for this rule.
         */
        template <class Annotation>
        annotation_parse_node<rule_ref_parse_node<ParseContext>, Annotation> 
            operator [](const Annotation& annotation)
        {
            return make_annotation_parse_node(*this, annotation);
        }

        /**
         * Returns a pointer to this rule, since operator & is taken for `logical AND operator`.
         * @return a pointer to this rule.
         */
        const rule* this_ptr() const {
            return this;
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
        bool parse(ParseContext& pc) const {
            const auto it = pc.m_rule_data.find(this);

            //initial entrance
            if (it == pc.m_rule_data.end()) {
                const rule_data_type initial_state = { pc.parse_position().iterator(), ParseContext::rule_state::none };
                auto [it1, ok] = pc.m_rule_data.insert(std::make_pair(this, initial_state));
                return _parse(pc, it1->second, initial_state);
            }

            //subsequent entrance; start new state if iterator has advanced from last time
            else if (it->second.iterator != pc.parse_position().iterator()) {
                const auto prev_state = it->second;
                it->second.iterator = pc.parse_position().iterator();
                it->second.state = ParseContext::rule_state::none;
                return _parse(pc, it->second, prev_state);
            }

            //left recursion found
            else {
                switch (it->second.state) {
                    case ParseContext::rule_state::none:
                        throw left_recursion{ this };

                    case ParseContext::rule_state::reject:
                        return false;

                    case ParseContext::rule_state::accept:
                        pc.m_terminal_parsing_allowed = true;
                        return true;
                }
            }

            throw std::logic_error("rule: parse: invalid state");
        }

        /**
         * Checks if this rule is the same rule as the given rule.
         * Using pointer checks like this: `this == &r` does not work 
         * due to `operator &` being overloaded.
         * @param r the other rule to check.
         * @return true if this and given rule are the same rules, false otherwise.
         */
        bool is_same(const rule<ParseContext>& r) const {
            return this == r.this_ptr();
        }

        /**
         * Returns the name of the rule.
         * @return the name of the rule.
         */
        std::string text() const override {
            return m_name;
        }

        /**
         * Returns the name of the rule.
         * @return the name of the rule.
         */
        const std::string& name() const {
            return m_name;
        }

        /**
         * Sets the name of the rule.
         * @param name the name of the rule.
         */
        void set_name(const std::string& name) {
            m_name = name;
        }

        #ifndef NDEBUG
        void init_tree() const override {
            if (m_parse_node) {
                m_parse_node->init();
            }
        }
        #endif

    private:
        //state
        std::unique_ptr<parse_node_wrapper<ParseContext>> m_parse_node;
        std::string m_name{"<unnamed rule>"};

        //create a wrapper for a parse node
        template <class ParseNode>
        auto create_wrapper(const ParseNode& parse_node) {
            return optimize_rule_parse_node<ParseContext>(*this, parse_node);
        }

        //create a wrapper for a rule
        auto create_wrapper(rule& r) {
            return make_unique_parse_node_wrapper<ParseContext>(make_parse_node(r));
        }

        //rule data type
        using rule_data_type = typename ParseContext::rule_data;

        //used for initiating a left recursion parsing
        struct left_recursion {
            const rule_type* rule;
        };

        //parse rule; on exception, restore rule data
        bool _parse(ParseContext& pc, rule_data_type& rd, const rule_data_type& prev_rd) const {
            try {
                const bool result = m_parse_node->parse(pc);
                rd = prev_rd;
                return result;
            }
            catch (left_recursion ex) {
                if (ex.rule == this) {
                    try {
                        const bool result = parse_left_recursion(pc, rd);
                        rd = prev_rd;
                        return result;
                    }
                    catch (...) {
                        rd = prev_rd;
                        throw;
                    }
                }
                else {
                    rd = prev_rd;
                    throw ex;
                }
            }
        }

        //parse left recursion
        bool parse_left_recursion(ParseContext& pc, rule_data_type& rd) const {
            const auto prev_left_recursion_start_state = pc.m_left_recursion_start_state;
            const bool prev_terminal_parsing_allowed = pc.m_terminal_parsing_allowed;

            pc.m_left_recursion_start_state = pc.get_state();

            //reject
            rd.state = ParseContext::rule_state::reject;
            if (!m_parse_node->parse(pc)) {
                return false;
            }

            //accept
            rd.state = ParseContext::rule_state::accept;
            for(;;) {
                rd.iterator = pc.m_parse_position.iterator();
                pc.m_terminal_parsing_allowed = false;
                try {
                    if (!m_parse_node->parse(pc)) {
                        break;
                    }
                }
                catch (...) {
                    pc.m_left_recursion_start_state = prev_left_recursion_start_state;
                    pc.m_terminal_parsing_allowed = prev_terminal_parsing_allowed;
                    throw;
                }
            }

            //success
            pc.m_left_recursion_start_state = prev_left_recursion_start_state;
            pc.m_terminal_parsing_allowed = prev_terminal_parsing_allowed;
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
