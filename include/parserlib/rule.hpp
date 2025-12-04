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
        rule(const std::string& name = "") :
            parse_node_base(name)
        {
        }

        rule(const rule&) = delete;

        rule(rule&&) = delete;

        template <class T> 
        rule(const T& value)
            : parse_node_base(make_parse_node(value).type())
            , m_parse_function([v = make_parse_node(value)](parse_context_interface& pc) { return v.parse(pc); })
        {
        }

        template <class T> 
        rule(const std::string& name, const T& value)
            : parse_node_base(name, make_parse_node(value).type())
            , m_parse_function([v = make_parse_node(value)](parse_context_interface& pc) { return v.parse(pc); })
        {
        }

        rule(rule& r)
            : parse_node_base("", "rule&")
            , m_parse_function([&](parse_context_interface& pc) { return r.parse(pc); })
        {
        }

        rule(const std::string& name, rule& r)
            : parse_node_base(name, "rule&")
            , m_parse_function([&](parse_context_interface& pc) { return r.parse(pc); })
        {
        }

        rule& operator = (const rule&) = delete;

        rule& operator = (rule&&) = delete;

        template <class T>
        rule& operator = (const T& value) {
            const auto v = make_parse_node(value);
            m_parse_function = [v](parse_context_interface& pc) { return v.parse(pc); };
            set_type(v.type());
            return *this;
        }

        rule& operator = (rule& r) {
            m_parse_function = [&](parse_context_interface& pc) { return r.parse(pc); };
            set_type("rule&");
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

        bool parse(parse_context_interface& pc) const {
            return m_parse_function(pc);
        }

    private:
        parse_function_type m_parse_function;
    };


    rule_parse_node::rule_parse_node(rule& r)
        : parse_node<rule_parse_node>(r.name())
        , m_rule(r)
    {
    }


    bool rule_parse_node::parse(parse_context_interface& pc) const {
        return m_rule.parse(pc);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
