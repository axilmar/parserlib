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

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_ONEORMORE_HPP
