#ifndef PARSERLIB_EXPRESSIONWRAPPER_HPP
#define PARSERLIB_EXPRESSIONWRAPPER_HPP


#include "IExpression.hpp"


namespace parserlib
{


    /**
        Expression wrapper.
        @param ParseContextType type of parse context.
        @param ExpressionType type of expression to wrap.
     */
    template <typename ParseContextType, typename ExpressionType> 
    class ExpressionWrapper : public IExpression<ParseContextType>
    {
    public:
        /**
            Constructor.
            @param expr expression.
         */
        ExpressionWrapper(ExpressionType&& expr)
            : m_expression(std::move(expr))
        {
        }

        /**
            Invokes the expression.
            @param pc parse context.
            @return parse success/failure.
         */
        virtual bool parse(ParseContextType& pc) const
        {
            return m_expression.parse(pc);
        }

    private:
        //expression to wrap
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_EXPRESSIONWRAPPER_HPP
