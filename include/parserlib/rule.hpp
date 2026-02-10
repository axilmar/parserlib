#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <set>
#include <map>
#include "grammar_node.hpp"


namespace parserlib {


    /**
     * Class used for assosiating a grammar expression to a variable.
     * It allows the use of a grammar expression in other expressions.
     * It allows for recursive grammars.
     * Internally, rule references are registered with a thread-local map,
     * and therefore constructing a grammar tree that uses a rule should only be done
     * in one thread.
     * Using rules for parsing can be done by multiple threads, since rules are 
     * considered immutable for parsing.
     */ 
    class rule {
    public:
        /**
         * The default constructor.
         * An empty rule is created.
         */ 
        rule() {
        }

        /**
         * The copy constructor.
         * Any references for this rule created before this constructor is invoked are updated
         * with the node of the given rule.
         * @param r the source rule.
         */ 
        rule(const rule& r)
            : m_node(r.m_node)
        {
            _update_refs();
        }

        /**
         * The move constructor.
         * The rule gets the node pointer and the references of the given rule.
         * @param r the source rule.
         */ 
        rule(rule&& r)
            : m_node(std::move(r.m_node))
        {
            _move_refs(std::addressof(r));
        }

        /**
         * Constructor from pointer to grammar.
         * Any references for this rule created before this constructor is invoked are updated
         * with the given node.
         * @param node the node to set to this rule.
         */ 
        rule(const grammar_node_ptr& node)
            : m_node(node)
        {
            _update_refs();
        }

        /**
         * Constructor from symbol.
         * It creates a symbol grammar node.
         * @param symbol the symbol to use.
         */ 
        template <class T>
        rule(const T& symbol)
            : m_node(std::make_shared<symbol_grammar_node>(symbol))
        {
            _update_refs();
        }

        /**
         * Constructor from symbol string.
         * It creates a string grammar node.
         * @param symbol the symbol string to use.
         */ 
        template <class T>
        rule(const T* symbol)
            : m_node(std::make_shared<string_grammar_node>(symbol))
        {
            _update_refs();
        }

        /**
         * Destroys the rule.
         * Any references to the rule are destroyed if there reference count reaches 0.
         */ 
        ~rule() {
            _remove_refs();
        }

        /**
         * The copy assignment operator.
         * This rule gets a shallow copy of the node of the given rule.
         * The references of this rule are also updated.
         * @param r the source object.
         * @return reference to this.
         */ 
        rule& operator = (const rule& r) {
            m_node = r.m_node;
            _update_refs();
            return *this;
        }

        /**
         * The move assignment operator.
         * This rule gets the given rule's pointer moved to this.
         * The references of the given rule are also moved to this.
         * @param r the source object.
         * @return reference to this.
         */ 
        rule& operator = (rule&& r) {
            m_node = std::move(r.m_node);
            _move_refs(std::addressof(r));
            return *this;
        }

        /**
         * Assignment from grammar node.
         * This rule gets a shallow copy of the given node.
         * The references of this rule are also updated.
         * @param r the source object.
         * @return reference to this.
         */ 
        rule& operator = (const grammar_node_ptr& node) {
            m_node = node;
            _update_refs();
            return *this;
        }

        /**
         * Assignment from symbol.
         * This rule gets a new instance of a symbol grammar node.
         * The references of this rule are also updated.
         * @param r the source object.
         * @return reference to this.
         */ 
        template <class T>
        rule& operator = (const T& symbol) {
            m_node = std::make_shared<symbol_grammar_node>(symbol);
            _update_refs();
            return *this;
        }

        /**
         * Assignment from symbol string.
         * This rule gets a new instance of a string grammar node.
         * The references of this rule are also updated.
         * @param r the source object.
         * @return reference to this.
         */ 
        template <class T>
        rule& operator = (const T* symbol) {
            m_node = std::make_shared<string_grammar_node>(symbol);
            _update_refs();
            return *this;
        }

        /**
         * Creates a reference node for this rule.
         * @return a reference node for this rule.
         */ 
        grammar_node_ptr create_ref_node() const {
            ref_grammar_node_ptr ref = std::make_shared<ref_grammar_node>(m_node);
            _get_rule_ref_map()[this].push_back(ref);
            return ref;
        }

        /**
         * Returns the rule of this node.
         */ 
        const grammar_node_ptr& get_node() const {
            return m_node;
        }

        /**
         * Creates a zero-or-more times loop for this rule.
         * @return a zero-or-more times loop for this rule.
         */  
        inline grammar_node_ptr operator *() const;

        /**
         * Creates a one-or-more times loop for this rule.
         * @return a one-or-more times loop for this rule.
         */  
        inline grammar_node_ptr operator +() const;

        /**
         * Makes this rule optional.
         * @return an optional node for this rule.
         */  
        inline grammar_node_ptr operator -() const;

        /**
         * Makes this rule a logical and predicate.
         * @return a logical and node for this rule.
         */  
        inline grammar_node_ptr operator &() const;

        /**
         * Makes this rule a logical not predicate.
         * @return a logical not node for this rule.
         */  
        inline grammar_node_ptr operator !() const;

        /**
         * Helper function for parsing.
         * @param pc the parse context to use.
         * @return true on success, false on error.
         */
        template <class ParseContext, class SymbolComparator = default_symbol_comparator>
        inline bool parse(ParseContext& pc) const;

    private:
        //the node
        grammar_node_ptr m_node;

        //tracks the references of a rule
        static std::map<const rule*, std::vector<ref_grammar_node_ptr>>& _get_rule_ref_map() {
            static thread_local std::map<const rule*, std::vector<ref_grammar_node_ptr>> map;
            return map;
        }

        //moves the references of a rule to this 
        void _move_refs(const rule* src_rule) {
            auto it = _get_rule_ref_map().find(src_rule);
            if (it != _get_rule_ref_map().end()) {
                _get_rule_ref_map()[this] = it->second;
                _get_rule_ref_map().erase(it);
            }
        }

        //updates the references with the current node
        void _update_refs() {
            const auto it = _get_rule_ref_map().find(this);
            if (it != _get_rule_ref_map().end()) {
                for (const ref_grammar_node_ptr& ref : it->second) {
                    ref->set_node(m_node);
                }
            }
        }

        //removes any refs for this node
        void _remove_refs() {
            _get_rule_ref_map().erase(this);
        }
    };


    inline grammar_node_ptr::grammar_node_ptr(const rule& r)
        : std::shared_ptr<grammar_node>(r.create_ref_node())
    {
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
