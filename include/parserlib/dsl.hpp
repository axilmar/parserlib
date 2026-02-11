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


    /**
     * Creates a function grammar node.
     * Used for custom parsing fnuctions.
     * @param f the function to invoke for parsing.
     * @return a function grammar node.
     */ 
    template <class T, std::enable_if_t<std::is_base_of_v<parse_context_base, T>, bool> = true>
    grammar_node_ptr function(const std::function<bool(T&)>& f) {
        return std::make_shared<function_grammar_node>(f);
    }


    /**** grammar node construction variables ****/


    /**
     * An any grammar node.
     * It parses any one symbol.
     */ 
    extern const grammar_node_ptr any;


    /**
     * An end grammar node.
     * Used for checking if the end of input has been reached.
     */ 
    extern const grammar_node_ptr end;


    /**
     * A false grammar node.
     */ 
    extern const grammar_node_ptr false_;


    /**
     * A true grammar node.
     * Used for returning false.
     */ 
    extern const grammar_node_ptr true_;


    /**** binary operators ****/


    /**
     * Creates a sequence out of two grammar nodes.
     * Sequence nodes are flattened.
     * @param left the left node.
     * @param right the right node.
     * @return a sequence grammar node.
     */ 
    grammar_node_ptr operator >> (const grammar_node_ptr& left, const grammar_node_ptr& right);


    /**
     * Creates a choice out of two grammar nodes.
     * Choice nodes are flattened.
     * @param left the left node.
     * @param right the right node.
     * @return a choice grammar node.
     */ 
    grammar_node_ptr operator | (const grammar_node_ptr& left, const grammar_node_ptr& right);

    /**
     * The exclusion operator.
     * It converts an expression of type `A - B` to the expression `!B >> A`.
     * @param left the node to parse.
     * @param right the node to use for test; if parsing this node returns true, then parsing of the left node does not occur.
     * @return a sequence node.
     */ 
    grammar_node_ptr operator - (const grammar_node_ptr& left, const grammar_node_ptr& right);


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


} //namespace parserlib


#endif //PARSERLIB_DSL_HPP
