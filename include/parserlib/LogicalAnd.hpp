#ifndef PARSERLIB_LOGICALAND_HPP
#define PARSERLIB_LOGICALAND_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A logical AND operator.
        The given expression can parse once, but it does not advance the input position.
        @param ExpressionType expression type to parse.
     */
    template <class ExpressionType> class LogicalAnd :
        public Expression,
        public UnaryOperatorsBase<LogicalAnd<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        LogicalAnd(const ExpressionType& expression) :
            m_expression(expression)
        {
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICALAND_HPP
