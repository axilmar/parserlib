#ifndef PARSERLIB_DSL_HPP
#define PARSERLIB_DSL_HPP


#include "rule.hpp"


namespace parserlib {


    /**** grammar node construction functions ****/


    template <class T>
    grammar_node_ptr terminal(const T& symbol) {
        return std::make_shared<symbol_grammar_node>(symbol);
    }


    template <class T>
    grammar_node_ptr terminal(const T* symbol) {
        return std::make_shared<string_grammar_node>(symbol);
    }


    template <class T>
    grammar_node_ptr set(const T* set) {
        return std::make_shared<set_grammar_node>(set);
    }


    template <class T>
    grammar_node_ptr set(const std::initializer_list<T>& set) {
        return std::make_shared<set_grammar_node>(set);
    }


    template <class T>
    grammar_node_ptr set(const std::vector<T>& set) {
        return std::make_shared<set_grammar_node>(set);
    }


    template <class T>
    grammar_node_ptr range(const T& min, const T& max) {
        return std::make_shared<range_grammar_node>(min, max);
    }


    inline grammar_node_ptr any() {
        return std::make_shared<any_grammar_node>();
    }


    inline grammar_node_ptr end() {
        return std::make_shared<end_grammar_node>();
    }


    inline grammar_node_ptr false_() {
        return std::make_shared<false_grammar_node>();
    }


    inline grammar_node_ptr true_() {
        return std::make_shared<true_grammar_node>();
    }


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


    inline grammar_node_ptr operator >> (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        std::shared_ptr<sequence_grammar_node> sequence_node = std::make_shared<sequence_grammar_node>();
        sequence_node->set_first(left->get_type() != grammar_node_type_sequence ? left : std::static_pointer_cast<sequence_grammar_node>(left)->get_first());
        sequence_node->get_first()->set_last(right->get_type() != grammar_node_type_sequence ? right : std::static_pointer_cast<sequence_grammar_node>(right)->get_first());
        return sequence_node;
    }


    inline grammar_node_ptr operator | (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        std::shared_ptr<choice_grammar_node> choice_node = std::make_shared<choice_grammar_node>();
        choice_node->set_first(left->get_type() != grammar_node_type_choice ? left : std::static_pointer_cast<choice_grammar_node>(left)->get_first());
        choice_node->get_first()->set_last(right->get_type() != grammar_node_type_choice ? right : std::static_pointer_cast<choice_grammar_node>(right)->get_first());
        return choice_node;
    }


    template <class T>
    grammar_node_ptr operator ->* (const grammar_node_ptr node, const T& id) {
        return std::make_shared<match_grammar_node>(node, id);
    }


    inline grammar_node_ptr operator - (const grammar_node_ptr& left, const grammar_node_ptr& right) {
        return !right >> left;
    }


} //namespace parserlib


#endif //PARSERLIB_DSL_HPP
