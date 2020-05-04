#ifndef PARSERLIB_ZEROORMORE_HPP
#define PARSERLIB_ZEROORMORE_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A zero-or-more operator.
        The given expression can parse zero or more times.
        @param ExpressionType expression type to parse in a loop.
     */
    template <typename ExpressionType> class ZeroOrMore :
        public Expression,
        public UnaryOperatorsBase<ZeroOrMore<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        ZeroOrMore(const ExpressionType& expression) :
            m_expression(expression)
        {
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_ZEROORMORE_HPP
