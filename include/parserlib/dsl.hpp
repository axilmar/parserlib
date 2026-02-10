#ifndef PARSERLIB_DSL_HPP
#define PARSERLIB_DSL_HPP


#include "rule.hpp"


namespace parserlib {


    /**** grammar node construction functions ****/


    /**
     * Constructs a symbol node.
     * @param symbol the symbol.
     * @return the symbol node.
     */ 
    template <class T>
    grammar_node_ptr terminal(const T& symbol) {
        return std::make_shared<symbol_grammar_node>(symbol);
    }


    /**
     * Constructs a symbol string node.
     * @param symbol the symbol.
     * @return the symbol string node.
     */ 
    template <class T>
    grammar_node_ptr terminal(const T* symbol) {
        return std::make_shared<string_grammar_node>(symbol);
    }


    /**
     * Constructs a symbol set node.
     * @param set the set.
     * @return the symbol set node.
     */ 
    template <class T>
    grammar_node_ptr set(const std::basic_string_view<T>& set) {
        return std::make_shared<set_grammar_node>(set);
    }


    /**
     * Constructs a symbol set node.
     * @param set the set.
     * @return the symbol set node.
     */ 
    template <class T>
    grammar_node_ptr set(const T* set) {
        return std::make_shared<set_grammar_node>(set);
    }


    /**
     * Constructs a symbol set node.
     * @param set the set.
     * @return the symbol set node.
     */ 
    template <class T>
    grammar_node_ptr set(const std::initializer_list<T>& set) {
        return std::make_shared<set_grammar_node>(set);
    }


    /**
     * Constructs a symbol set node.
     * @param set the set.
     * @return the symbol set node.
     */ 
    template <class T>
    grammar_node_ptr set(const std::vector<T>& set) {
        return std::make_shared<set_grammar_node>(set);
    }


    /**
     * Constructs a symbol range node.
     * @param min the min value.
     * @param max the max value.
     * @return the symbol range node.
     */ 
    template <class T>
    grammar_node_ptr range(const T& min, const T& max) {
        return std::make_shared<range_grammar_node>(min, max);
    }


    /**** grammar node construction variables ****/


    /**
     * An any grammar node.
     * It parses any one symbol.
     */ 
    inline const grammar_node_ptr any = std::make_shared<any_grammar_node>();


    /**
     * An end grammar node.
     * Used for checking if the end of input has been reached.
     */ 
    inline const grammar_node_ptr end = std::make_shared<end_grammar_node>();


    /**
     * A false grammar node.
     */ 
    inline const grammar_node_ptr false_ = std::make_shared<false_grammar_node>();


    /**
     * A true grammar node.
     * Used for returning false.
     */ 
    inline const grammar_node_ptr true_ = std::make_shared<true_grammar_node>();


    /**
     * Creates a function grammar node.
     * Used for custom parsing fnuctions.
     * @param f the function to invoke for parsing.
     * @return a function grammar node.
     */ 
    template <class T, std::enable_if_t<std::is_base_of_v<parse_context_base, T>, bool> = true>
    inline grammar_node_ptr function(const std::function<bool(T&)>& f) {
        return std::make_shared<function_grammar_node>(f);
    }


    /**** unary operators for grammar_node_ptr ****/


    inline grammar_node_ptr grammar_node_ptr::operator *() const {
        return std::make_shared<loop_0_grammar_node>(*this);
    }


    inline grammar_node_ptr grammar_node_ptr::operator +() const {
        return std::make_shared<loop_1_grammar_node>(*this);
    }


    inline grammar_node_ptr grammar_node_ptr::operator -() const {
        return std::make_shared<optional_grammar_node>(*this);
    }


    inline grammar_node_ptr grammar_node_ptr::operator &() const {
        return std::make_shared<logical_and_grammar_node>(*this);
    }


    inline grammar_node_ptr grammar_node_ptr::operator !() const {
        return std::make_shared<logical_not_grammar_node>(*this);
    }


    /**** unary operators for rule ****/


    inline grammar_node_ptr rule::operator *() const {
        return std::make_shared<loop_0_grammar_node>(grammar_node_ptr(*this));
    }


    inline grammar_node_ptr rule::operator +() const {
        return std::make_shared<loop_1_grammar_node>(grammar_node_ptr(*this));
    }


    inline grammar_node_ptr rule::operator -() const {
        return std::make_shared<optional_grammar_node>(grammar_node_ptr(*this));
    }


    inline grammar_node_ptr rule::operator &() const {
        return std::make_shared<logical_and_grammar_node>(grammar_node_ptr(*this));
    }


    inline grammar_node_ptr rule::operator !() const {
        return std::make_shared<logical_not_grammar_node>(grammar_node_ptr(*this));
    }


    /**** binary operators ****/


    //helper function
    inline void add_node(grammar_node_container& nodes, grammar_node_type type, const grammar_node_ptr& child) {
        if (child->get_type() == type) {
            const parent_grammar_node* child_container = static_cast<const parent_grammar_node*>(child.get());
            nodes.insert(nodes.end(), child_container->get_children().begin(), child_container->get_children().end());
        }
        else {
            nodes.push_back(child);
        }
    }


    /**
     * Creates a sequence out of two grammar nodes.
     * Sequence nodes are flattened.
     * @param left the left node.
     * @param right the right node.
     * @return a sequence grammar node.
     */ 
    inline grammar_node_ptr operator >> (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        grammar_node_container nodes;
        add_node(nodes, grammar_node_type_sequence, left);
        add_node(nodes, grammar_node_type_sequence, right);
        return std::make_shared<sequence_grammar_node>(std::move(nodes));
    }


    /**
     * Creates a choice out of two grammar nodes.
     * Choice nodes are flattened.
     * @param left the left node.
     * @param right the right node.
     * @return a choice grammar node.
     */ 
    inline grammar_node_ptr operator | (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        grammar_node_container nodes;
        add_node(nodes, grammar_node_type_choice, left);
        add_node(nodes, grammar_node_type_choice, right);
        return std::make_shared<choice_grammar_node>(std::move(nodes));
    }



    /**
     * Creates a match grammar node.
     * @param node the node to create a match node for.
     * @param id the match id.
     * @return a match grammar node.
     */ 
    template <class T>
    grammar_node_ptr operator ->* (const grammar_node_ptr node, const T& id) {
        return std::make_shared<match_grammar_node>(node, id);
    }


    /**
     * The exclusion operator.
     * It converts an expression of type `A - B` to the expression `!B >> A`.
     * @param left the node to parse.
     * @param right the node to use for test; if parsing this node returns true, then parsing of the left node does not occur.
     * @return a sequence node.
     */ 
    inline grammar_node_ptr operator - (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        return !right >> left;
    }


} //namespace parserlib


#endif //PARSERLIB_DSL_HPP
