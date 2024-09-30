#ifndef PARSERLIB_RULEREFERENCE_HPP
#define PARSERLIB_RULEREFERENCE_HPP


#include "Parser.hpp"


namespace parserlib {


    template <class ParseContext>
    class Rule;


    /**
     * A reference to a rule.
     * @param ParseContext the parse context to use for a rule.
     */
    template <class ParseContext>
    class RuleReference : public Parser<RuleReference<ParseContext>> {
    public:
        /**
         * The default constructor.
         * @param rule the reference to rule.
         */
        RuleReference(Rule<ParseContext>& rule)
            : m_rule(rule)
        {
        }

        /**
         * Invokes the rule to parse.
         * @return whatever the rule returns.
         */
        bool parse(ParseContext& pc) const {
            return m_rule.parse(pc);
        }

    private:
        Rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULEREFERENCE_HPP
