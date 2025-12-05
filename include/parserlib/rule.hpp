#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include "parserlib/loop_0_parse_node.hpp"
#include "parserlib/loop_1_parse_node.hpp"
#include "parserlib/optional_parse_node.hpp"
#include "parserlib/logical_and_parse_node.hpp"
#include "parserlib/logical_not_parse_node.hpp"
#include "rule_parse_node.hpp"


namespace parserlib {


    class rule : public parse_node_base {
    public:
        rule() {
        }

        rule(const rule&) = delete;

        rule(rule&&) = delete;

        template <class T> 
        rule(const T& value)
            : m_parse_function([v = make_parse_node(value)](parse_context_interface& pc) { return v.parse(pc); })
        {
        }

        rule(rule& r)
            : m_parse_function([&](parse_context_interface& pc) { return r.parse(pc); })
        {
        }

        rule& operator = (const rule&) = delete;

        rule& operator = (rule&&) = delete;

        template <class T>
        rule& operator = (const T& value) {
            m_parse_function = [v = make_parse_node(value)](parse_context_interface& pc) { return v.parse(pc); };
            return *this;
        }

        rule& operator = (rule& r) {
            m_parse_function = [&](parse_context_interface& pc) { return r.parse(pc); };
            return *this;
        }

        loop_0_parse_node<rule_parse_node> operator *() {
            return rule_parse_node(*this);
        }

        loop_1_parse_node<rule_parse_node> operator +() {
            return rule_parse_node(*this);
        }

        optional_parse_node<rule_parse_node> operator -() {
            return rule_parse_node(*this);
        }

        logical_and_parse_node<rule_parse_node> operator &() {
            return rule_parse_node(*this);
        }

        logical_not_parse_node<rule_parse_node> operator !() {
            return rule_parse_node(*this);
        }

        bool parse(parse_context_interface& pc) {
            return pc.parse_left_recursion(this, m_parse_function);
        }

    private:
        parse_function_type m_parse_function;
    };


    rule_parse_node::rule_parse_node(rule& r)
        : m_rule(r)
    {
    }


    bool rule_parse_node::parse(parse_context_interface& pc) const {
        return m_rule.parse(pc);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
