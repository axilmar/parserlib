#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <functional>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "ParseContext.hpp"
#include "RuleReference.hpp"


namespace parserlib
{


    /**
        Class that represents a production rule.
        @param ParseContextType parse context.
     */
    template <typename ParseContextType = ParseContext<>> class Rule :
        public Expression,
        public UnaryOperatorsBase<Rule<ParseContextType>>
    {
    public:
        /**
            Constructor.
            @param expr expression.
         */
        template <typename ExpressionType>
        Rule(ExpressionType&& expression) :
            m_expression(
                [expr = std::move(expression)](ParseContextType& pc)
                {
                    return expr.parse(pc);
                })
        {
        }

        /**
            Constructor.
            @param rule rule reference.
         */
        Rule(Rule& rule) :
            m_expression(
                [ruleRef = RuleReference<ParseContextType>(rule)](ParseContextType& pc)
                {
                    return ruleRef.parse(pc);
                })
        {
        }

        /**
            Parses the input with the given rule.
            @param pc parse context.
            @return true on success, false on failure.
         */
        bool parse(ParseContextType& pc)
        {
            //TODO
            return false;
        }

    private:
        //grammar
        std::function<bool(ParseContextType&)> m_expression;
    };


    /**
        Specialization of expression type for Rule.
        It yields an object of type RuleReference<ParseContext>,
        in order to allow recursion in declarative style.
        @param ParseContextType parse context type.
     */
    template <typename ParseContextType> class ExpressionType<Rule<ParseContextType>>
    {
    public:
        ///type of expression for rule is rule reference; it enables recursion in declarative style.
        typedef RuleReference<ParseContextType> Type;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
