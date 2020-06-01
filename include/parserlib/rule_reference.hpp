#ifndef PARSERLIB__RULE_REFERENCE__HPP
#define PARSERLIB__RULE_REFERENCE__HPP


#include "expression_with_unary_operators.hpp"


namespace parserlib
{


    template <typename ParseContext> class rule;


    /**
        Class that represents a reference to a rule.        
        References to rules allow recursive rule declarations.
        @param ParseContext type of parse context to use in parsing.
     */
    template <typename ParseContext>
    class rule_reference : public expression_with_unary_operators<rule_reference<ParseContext>>
    {
    public:
        /**
            Constructor.
            @param rule rule reference.
         */
        rule_reference(rule<ParseContext>& rule)
            : m_rule(rule)
        {
        }

    private:
        //reference
        rule<ParseContext>& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB__RULE_REFERENCE__HPP