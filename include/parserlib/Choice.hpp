#ifndef PARSERLIB_CHOICE_HPP
#define PARSERLIB_CHOICE_HPP


#include <type_traits>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A choice of expressions.
        Either the left or the right side expression shall parse successfully.
        @param LeftExpressionType type of left expression.
        @param RightExpressionType type of right expression.
     */
    template <typename LeftExpressionType, typename RightExpressionType> 
    class Choice : 
        public Expression, 
        public UnaryOperatorsBase<Choice<LeftExpressionType, RightExpressionType>>
    {
    public:
        /**
            Constructor.
            @param left left-side expression.
            @param right right-side expression.
         */
        Choice(const LeftExpressionType& left, const RightExpressionType& right) :
            m_leftExpression(left), 
            m_rightExpression(right)
        {
        }

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            const auto startPosition = pc.getCurrentPosition();

            //try the left expression
            if (m_leftExpression.parse(pc))
            {
                return true;
            }

            //restore the current position since the left expression failed
            pc.setCurrentPosition(startPosition);

            //try the right expression since the left expression failed
            return m_rightExpression.parse(pc);
        }

    private:
        //left/right expressions
        const LeftExpressionType m_leftExpression;
        const RightExpressionType m_rightExpression;
    };


    /**
        Creates a choice out of a left expression and out of right expression,
        if they are expressions for this library or if there is a conversion
        of the given types to an expression of this library via the trait ExpressionType.
        @param left left-side expression.
        @param right right-side expression.
        @return a choice of expressions.
     */
    template <
        typename LeftExpressionType, 
        typename RightExpressionType,
        typename = std::enable_if_t<
            std::is_base_of_v<Expression, ExpressionTypeT<std::decay_t<LeftExpressionType>>> && 
            std::is_base_of_v<Expression, ExpressionTypeT<std::decay_t<RightExpressionType>>>>> 
    Choice<
        ExpressionTypeT<std::decay_t<LeftExpressionType>>, 
        ExpressionTypeT<std::decay_t<RightExpressionType>>> 
        operator | (LeftExpressionType&& left, RightExpressionType&& right)
    {
        return {
            ExpressionTypeT<std::decay_t<LeftExpressionType>>(left), 
            ExpressionTypeT<std::decay_t<RightExpressionType>>(right)};
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_HPP
