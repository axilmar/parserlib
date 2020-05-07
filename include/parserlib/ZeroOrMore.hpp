#ifndef PARSERLIB_ZEROORMORE_HPP
#define PARSERLIB_ZEROORMORE_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "parseLoop.hpp"


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

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            parseLoop(m_expression, pc);
            return true;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB_ZEROORMORE_HPP