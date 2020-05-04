#ifndef PARSERLIB_OPTIONAL_HPP
#define PARSERLIB_OPTIONAL_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        An optional operator.
        The given expression can parse once optionally.
        @param ExpressionType expression type to parse optionally.
     */
    template <typename ExpressionType> class Optional :
        public Expression,
        public UnaryOperatorsBase<Optional<ExpressionType>>
    {
    public:
        /**
            constructor.
            @param expression expression to parse.
         */
        Optional(const ExpressionType& expression) :
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
            m_expression.parse(pc);
            return true;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_HPP
