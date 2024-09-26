#ifndef PARSERLIB_RULEREFERENCE_HPP
#define PARSERLIB_RULEREFERENCE_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class ParseContext>
    class Rule;


    template <class ParseContext>
    class RuleReference : public Parser<RuleReference<ParseContext>> {
    public:
        RuleReference(Rule<ParseContext>& rule)
            : m_rule(rule)
        {
        }

        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        Rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULEREFERENCE_HPP
