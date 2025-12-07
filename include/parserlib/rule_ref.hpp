#ifndef PARSERLIB_RULE_REF_HPP
#define PARSERLIB_RULE_REF_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class ParseContext>
    class rule_ref : public parse_node<rule_ref<ParseContext>> {
    public:
        using rule_type = rule<ParseContext>;

        rule_ref(rule_type& rule) : m_rule(rule) {
        }

        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        rule_type& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_REF_HPP
