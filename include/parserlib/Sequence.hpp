#ifndef PARSERLIB_SEQUENCE_HPP
#define PARSERLIB_SEQUENCE_HPP


#include <type_traits>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A sequence of expressions.
        All expressions must parse successfully.
        @param LeftExpressionType type of left expression.
        @param RightExpressionType type of right expression.
     */
    template <typename LeftExpressionType, typename RightExpressionType> 
    class Sequence : 
        public Expression, 
        public UnaryOperatorsBase<Sequence<LeftExpressionType, RightExpressionType>>
    {
    public:
        /**
            Constructor.
            @param left left-side expression.
            @param right right-side expression.
         */
        Sequence(const LeftExpressionType& left, const RightExpressionType& right) :
            m_leftExpression(left), 
            m_rightExpression(right)
        {
        }

    private:
        const LeftExpressionType m_leftExpression;
        const RightExpressionType m_rightExpression;
    };


    /**
        Creates a sequence out of a left expression and out of right expression,
        if they are expressions for this library or if there is a conversion
        of the given types to an expression of this library via the trait ExpressionType.
        @param left left-side expression.
        @param right right-side expression.
        @return a sequence of expressions.
     */
    template <
        typename LeftExpressionType, 
        typename RightExpressionType,
        typename = std::enable_if_t<
            std::is_base_of_v<Expression, ExpressionTypeT<std::decay_t<LeftExpressionType>>> && 
            std::is_base_of_v<Expression, ExpressionTypeT<std::decay_t<RightExpressionType>>>>> 
    Sequence<
        ExpressionTypeT<std::decay_t<LeftExpressionType>>, 
        ExpressionTypeT<std::decay_t<RightExpressionType>>> 
        operator >> (LeftExpressionType&& left, RightExpressionType&& right)
    {
        return {
            ExpressionTypeT<std::decay_t<LeftExpressionType>>(left), 
            ExpressionTypeT<std::decay_t<RightExpressionType>>(right)};
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_HPP
