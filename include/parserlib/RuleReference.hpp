#ifndef PARSERLIB_RULEREFERENCE_HPP
#define PARSERLIB_RULEREFERENCE_HPP


namespace parserlib
{


    template <typename ParseContextType> class Rule;


    /**
        Wrapper class around a rule reference.
        @param ParseContextType parse context type.
     */
    template <typename ParseContextType> class RuleReference :
        public Expression,
        public UnaryOperatorsBase<RuleReference<ParseContextType>>
    {
    public:
        /**
            Constructor.
            @param rule rule reference.
         */
        RuleReference(Rule<ParseContextType>& rule) :
            m_rule(rule)
        {
        }

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        bool parse(ParseContextType& pc) const
        {
            return m_rule._parseLR(pc);
        }

    private:
        //rule reference.
        Rule<ParseContextType>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULEREFERENCE_HPP
