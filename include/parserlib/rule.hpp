#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include "generic_source.hpp"
#include "parse_context.hpp"
#include "parse_node_tag.hpp"
#include "rule_ref.hpp"
#include "loop0.hpp"
#include "loop1.hpp"
#include "optional.hpp"
#include "logical_and.hpp"
#include "logical_not.hpp"
#include "make_parse_node.hpp"


namespace parserlib {


    template <class ParseContext = parse_context<generic_source<>>>
    class rule : public parse_node_tag {
    public:
        using parse_context_type = ParseContext;
        using parse_function_type = typename ParseContext::parse_function_type;
        using rule_ref_type = rule_ref<ParseContext>;

        rule() {
        }

        rule(const rule&) = delete;
        rule(rule&&) = delete;

        template <class T>
        rule(T&& expr)
            : m_parse_function(_make_parse_function(expr))
        {
        }

        rule& operator =(const rule&) = delete;
        rule& operator =(rule&&) = delete;

        loop0<rule_ref_type> operator *() { return *this; }
        loop1<rule_ref_type> operator +() { return *this; }
        optional<rule_ref_type> operator -() { return *this; }
        logical_and<rule_ref_type> operator &() { return *this; }
        logical_not<rule_ref_type> operator !() { return *this; }

        template <class T>
        rule& operator =(T&& expr) {
            m_parse_function = _make_parse_function(expr);
            return *this;
        }

        bool parse(ParseContext& pc) {
            return pc.parse_rule(*this, m_parse_function);
        }

    private:
        parse_function_type m_parse_function;

        template <class T>
        parse_function_type _make_parse_function(T&& expr) {
            return [pn = make_parse_node(expr)](ParseContext& pc) { return pn.parse(pc); };
        }
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
