#include "parserlib/grammar_node.hpp"


namespace parserlib {


    /**** class grammar_node ****/


    grammar_node::~grammar_node() {
    }


    grammar_node_type grammar_node::get_type() const {
        return m_type;
    }


    grammar_node::grammar_node(grammar_node_type type) 
        : m_type(type)
    {
    }


    /**** class grammar_node_ptr ****/


    grammar_node_ptr::grammar_node_ptr(const std::shared_ptr<grammar_node>& node)
        : std::shared_ptr<grammar_node>(node)
    {
    }


    grammar_node_ptr grammar_node_ptr::operator *() const {
        return std::make_shared<loop_0_grammar_node>(*this);
    }


    grammar_node_ptr grammar_node_ptr::operator +() const {
        return std::make_shared<loop_1_grammar_node>(*this);
    }


    grammar_node_ptr grammar_node_ptr::operator -() const {
        return std::make_shared<optional_grammar_node>(*this);
    }


    grammar_node_ptr grammar_node_ptr::operator &() const {
        return std::make_shared<logical_and_grammar_node>(*this);
    }


    grammar_node_ptr grammar_node_ptr::operator !() const {
        return std::make_shared<logical_not_grammar_node>(*this);
    }


    /**** class parent_grammar_node_single_child ****/


    const grammar_node_ptr& parent_grammar_node_single_child::get_child() const {
        return m_child;
    }


    parent_grammar_node_single_child::parent_grammar_node_single_child(grammar_node_type type, const grammar_node_ptr& child)
        : grammar_node(type)
        , m_child(child)
    {
    }


    /**** class parent_grammar_node ****/


    const grammar_node_container& parent_grammar_node::get_children() const {
        return m_children;
    }


    parent_grammar_node::parent_grammar_node(grammar_node_type type, grammar_node_container&& children)
        : grammar_node(type)
        , m_children(std::move(children))
    {
    }


    /**** class ref_grammar_node ****/


    ref_grammar_node::ref_grammar_node(const grammar_node_ptr& node) 
        : grammar_node(grammar_node_type_ref) 
        , m_node(node)
    {
    }


    const grammar_node_ptr ref_grammar_node::get_node() const {
        return m_node.lock();
    }


    void ref_grammar_node::set_node(const grammar_node_ptr& node) {
        m_node = node;
    }


    /**** class symbol_grammar_node ****/


    const symbol_ptr& symbol_grammar_node::get_symbol() const {
        return m_symbol;
    }


    /**** class string_grammar_node ****/


    const symbol_string_ptr& string_grammar_node::get_symbol() const {
        return m_symbol;
    }


    /**** class set_grammar_node ****/


    const symbol_set_ptr& set_grammar_node::get_symbol() const {
        return m_symbol;
    }


    /**** class range_grammar_node ****/


    const symbol_pair_ptr& range_grammar_node::get_symbol() const {
        return m_symbol;
    }


    /**** class loop_0_grammar_node ****/


    loop_0_grammar_node::loop_0_grammar_node(const grammar_node_ptr& child)
        : parent_grammar_node_single_child(grammar_node_type_loop_0, child)
    {
    }


    /**** class loop_1_grammar_node ****/


    loop_1_grammar_node::loop_1_grammar_node(const grammar_node_ptr& child)
        : parent_grammar_node_single_child(grammar_node_type_loop_1, child)
    {
    }


    /**** class optional_grammar_node ****/


    optional_grammar_node::optional_grammar_node(const grammar_node_ptr& child)
        : parent_grammar_node_single_child(grammar_node_type_optional, child)
    {
    }


    /**** class logical_and_grammar_node ****/


    logical_and_grammar_node::logical_and_grammar_node(const grammar_node_ptr& child)
        : parent_grammar_node_single_child(grammar_node_type_logical_and, child)
    {
    }


    /**** class logical_not_grammar_node ****/


    logical_not_grammar_node::logical_not_grammar_node(const grammar_node_ptr& child)
        : parent_grammar_node_single_child(grammar_node_type_logical_not, child)
    {
    }


    /**** class sequence_grammar_node ****/


    sequence_grammar_node::sequence_grammar_node(grammar_node_container&& children) : 
        parent_grammar_node(grammar_node_type_sequence, std::move(children))
    {
    }


    /**** class choice_grammar_node ****/


    choice_grammar_node::choice_grammar_node(grammar_node_container&& children) 
        : parent_grammar_node(grammar_node_type_choice, std::move(children))
    {
    }


    /**** class match_grammar_node ****/


    const symbol_ptr& match_grammar_node::get_id_symbol() const {
        return m_id_symbol;
    }


    /**** class any_grammar_node ****/


    any_grammar_node::any_grammar_node() 
        : grammar_node(grammar_node_type_any)
    {
    }


    /**** class end_grammar_node ****/


    end_grammar_node::end_grammar_node() 
        : grammar_node(grammar_node_type_end)
    {
    }


    /**** class false_grammar_node ****/


    false_grammar_node::false_grammar_node() 
        : grammar_node(grammar_node_type_false)
    {
    }


    /**** class any_grammar_node ****/


    true_grammar_node::true_grammar_node() 
        : grammar_node(grammar_node_type_true)
    {
    }


    /**** class function_grammar_node ****/


    const parse_function& function_grammar_node::get_function() const {
        return m_function;
    }


} //namespace parserlib
