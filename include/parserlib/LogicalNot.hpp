#ifndef PARSERLIB_LOGICALNOT_HPP
#define PARSERLIB_LOGICALNOT_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A logical NOT operator.
        The given expression can parse once, but it does not advance the input position.
        @param ExpressionType expression type to parse.
     */
    template <typename ExpressionType> class LogicalNot :
        public Expression,
        public UnaryOperatorsBase<LogicalNot<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        LogicalNot(const ExpressionType& expression) :
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
            const bool result = !m_expression.parse(pc);
            pc.setCurrentPosition(startPosition);
            return result;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICALNOT_HPP
