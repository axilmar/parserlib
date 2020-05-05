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
    template <typename ExpressionType> class LogicalAnd :
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

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            const auto startPosition = pc.getCurrentPosition();
            const auto startOutputState = pc.getOutputState();
            const bool result = m_expression.parse(pc);
            pc.setCurrentPosition(startPosition);
            pc.setOutputState(startOutputState);
            return result;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICALAND_HPP
