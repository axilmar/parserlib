#ifndef PARSERLIB_ONEORMORE_HPP
#define PARSERLIB_ONEORMORE_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        An one-or-more operator.
        The given expression can parse one or more times.
        @param ExpressionType expression type to parse in a loop.
     */
    template <typename ExpressionType> class OneOrMore :
        public Expression,
        public UnaryOperatorsBase<OneOrMore<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        OneOrMore(const ExpressionType& expression) :
            m_expression(expression)
        {
        }

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            //parse once
            if (m_expression.parse(pc))
            {
                return true;
            }

            //parse multiple types
            while (m_expression.parse(pc))
            {
            }

            return true;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_ONEORMORE_HPP
