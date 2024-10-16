#ifndef PARSERLIB_CORE_RULEREFERENCE_HPP
#define PARSERLIB_CORE_RULEREFERENCE_HPP


#include "Parser.hpp"


namespace parserlib::core {


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


    /**
     * Creates a reference to a rule.
     * @param r rule to refer to.
     * @return a reference to a rule.
     */
    template <class ParseContext>
    RuleReference<ParseContext> ref(Rule<ParseContext>& r) {
        return RuleReference<ParseContext>(r);
    }


} //namespace parserlib::core


#endif //PARSERLIB_CORE_RULEREFERENCE_HPP
