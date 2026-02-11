#include "parserlib/dsl.hpp"


namespace parserlib {


    /**** internal functions ****/


    //helper function
    static void add_node(grammar_node_container& nodes, grammar_node_type type, const grammar_node_ptr& child) {
        if (child->get_type() == type) {
            const parent_grammar_node* child_container = static_cast<const parent_grammar_node*>(child.get());
            nodes.insert(nodes.end(), child_container->get_children().begin(), child_container->get_children().end());
        }
        else {
            nodes.push_back(child);
        }
    }


    /**** grammar node construction variables ****/


    const grammar_node_ptr any = std::make_shared<any_grammar_node>();


    const grammar_node_ptr end = std::make_shared<end_grammar_node>();


    const grammar_node_ptr false_ = std::make_shared<false_grammar_node>();


    const grammar_node_ptr true_ = std::make_shared<true_grammar_node>();


    /**** binary operators ****/


    grammar_node_ptr operator >> (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        grammar_node_container nodes;
        add_node(nodes, grammar_node_type_sequence, left);
        add_node(nodes, grammar_node_type_sequence, right);
        return std::make_shared<sequence_grammar_node>(std::move(nodes));
    }


    grammar_node_ptr operator | (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        grammar_node_container nodes;
        add_node(nodes, grammar_node_type_choice, left);
        add_node(nodes, grammar_node_type_choice, right);
        return std::make_shared<choice_grammar_node>(std::move(nodes));
    }


    grammar_node_ptr operator - (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        return !right >> left;
    }


} //namespace parserlib
