#ifndef PARSERLIB_RULEREFERENCE_HPP
#define PARSERLIB_RULEREFERENCE_HPP


#include "ParserNode.hpp"


namespace parserlib {


    template <class ParseContextType> class Rule;


    /**
     * A reference to a rule.
     * Rules are the only named expressions in the grammar,
     * and therefore they are allowed to be recursive,
     * and so they are passed around via rule references.
     * @param ParseContextType type of context to pass to the parse function.
     */
    template <class ParseContextType> class RuleReference
        : public ParserNode<RuleReference<ParseContextType>> {
    public:
        /**
         * Constructor.
         * @param rule reference to rule.
         */
        RuleReference(Rule<ParseContextType>& rule) 
            : m_rule(rule) {
        }

        /**
         * Returns the rule.
         * @return the rule.
         */
        Rule<ParseContextType>& rule() const {
            return m_rule;
        }

        /**
         * Invokes the underlying rule.
         * @param pc parse context.
         * @return whatever the rule returns.
         */
        bool operator ()(ParseContextType& pc) const {
            return m_rule(pc);
        }

    private:
        Rule<ParseContextType>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULEREFERENCE_HPP
