#ifndef PARSERLIB_RULEEXPRESSION_HPP
#define PARSERLIB_RULEEXPRESSION_HPP


#include <string>
#include "Expression.hpp"


namespace parserlib
{


    /**
        Base class for rules.
     */
    class RuleExpression : public Expression
    {
    public:
        /**
            Returns the name of the rule.
            @return the name of the rule.
         */
        const std::string& getName() const
        {
            return m_name;
        }

    protected:
        /**
            Constructor.
            @param name name of the rule.
         */
        RuleExpression(const std::string& name = std::string()) :
            m_name(name)
        {
        }

    private:
        //name
        std::string m_name;
    };


} //namespace parserlib


#endif //PARSERLIB_RULEEXPRESSION_HPP
