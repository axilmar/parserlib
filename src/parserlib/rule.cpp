#include "parserlib/rule.hpp"


namespace parserlib {


    /**** internal functions ****/


    static std::map<const rule*, std::vector<ref_grammar_node_ptr>>& _get_rule_ref_map() {
        static thread_local std::map<const rule*, std::vector<ref_grammar_node_ptr>> map;
        return map;
    }


    static std::set<const rule*>& _get_constructed_rule_set() {
        static thread_local std::set<const rule*> set;
        return set;
    }


    static bool _is_constructed(const rule* r) {
        return _get_constructed_rule_set().find(r) != _get_constructed_rule_set().end();
    }


    static void _set_constructed(const rule* r) {
        _get_constructed_rule_set().insert(r);
    }


    static void _reset_constructed(const rule* r) {
        _get_constructed_rule_set().erase(r);
    }


    static void _move_refs(const rule* r, const rule* src_rule) {
        auto it = _get_rule_ref_map().find(src_rule);
        if (it != _get_rule_ref_map().end()) {
            _get_rule_ref_map()[r] = it->second;
            _get_rule_ref_map().erase(it);
        }
    }


    static void _update_refs(const rule* r) {
        const auto it = _get_rule_ref_map().find(r);
        if (it != _get_rule_ref_map().end()) {
            for (const ref_grammar_node_ptr& ref : it->second) {
                ref->set_node(r->get_node());
            }
        }
    }

    static void _remove_refs(const rule* r) {
        _get_rule_ref_map().erase(r);
    }


    /**** class rule ****/


    rule::rule() {
        _set_constructed(this);
    }


    rule::rule(const rule& r)
        : m_node(r.m_node)
    {
        _set_constructed(this);
        _update_refs(this);
    }


    rule::rule(rule&& r)
        : m_node(std::move(r.m_node))
    {
        _set_constructed(this);
        _move_refs(this, std::addressof(r));
    }


    rule::rule(const grammar_node_ptr& node)
        : m_node(node)
    {
        _set_constructed(this);
        _update_refs(this);
    }


    rule::~rule() {
        _remove_refs(this);
        _reset_constructed(this);
    }


    rule& rule::operator = (const rule& r) {
        m_node = r.m_node;
        _update_refs(this);
        return *this;
    }


    rule& rule::operator = (rule&& r) {
        m_node = std::move(r.m_node);
        _move_refs(this, std::addressof(r));
        return *this;
    }


    rule& rule::operator = (const grammar_node_ptr& node) {
        m_node = node;
        _update_refs(this);
        return *this;
    }


    grammar_node_ptr rule::create_ref_node() const {
        ref_grammar_node_ptr ref = std::make_shared<ref_grammar_node>(_is_constructed(this) ? m_node : grammar_node_ptr());
        _get_rule_ref_map()[this].push_back(ref);
        return ref;
    }


    const grammar_node_ptr& rule::get_node() const {
        return m_node;
    }


    grammar_node_ptr rule::operator *() const {
        return std::make_shared<loop_0_grammar_node>(grammar_node_ptr(*this));
    }


    grammar_node_ptr rule::operator +() const {
        return std::make_shared<loop_1_grammar_node>(grammar_node_ptr(*this));
    }


    grammar_node_ptr rule::operator -() const {
        return std::make_shared<optional_grammar_node>(grammar_node_ptr(*this));
    }


    grammar_node_ptr rule::operator &() const {
        return std::make_shared<logical_and_grammar_node>(grammar_node_ptr(*this));
    }


    grammar_node_ptr rule::operator !() const {
        return std::make_shared<logical_not_grammar_node>(grammar_node_ptr(*this));
    }


    void rule::_init() {
        _set_constructed(this);
        _update_refs(this);
    }


    void rule::_update() {
        _update_refs(this);
    }


    /**** class grammar_node_ptr ****/


    grammar_node_ptr::grammar_node_ptr(const rule& r)
        : std::shared_ptr<grammar_node>(r.create_ref_node())
    {
    }


} //namespace parserlib
