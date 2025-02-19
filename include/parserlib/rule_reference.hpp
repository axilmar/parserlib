#ifndef PARSERLIB_RULE_REFERENCE_HPP
#define PARSERLIB_RULE_REFERENCE_HPP


#include "parser.hpp"


namespace parserlib {


    template <class ParseContext> class rule;


    template <class ParseContext> 
    class rule_reference : public parser<rule_reference<ParseContext>> {
    public:
        using rule_type = rule<ParseContext>;

        rule_reference(const rule_type& rule)
            : m_rule(rule)
        {
        }

        bool parse(ParseContext& context) const {
            return m_rule.parse(context);
        }

        bool parse_left_recursion_start(ParseContext& context) const {
            return m_rule.parse_left_recursion_start(context);
        }

        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return m_rule.parse_left_recursion_continuation(context, match_start_state);
        }

    private:
        const rule_type& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_REFERENCE_HPP
