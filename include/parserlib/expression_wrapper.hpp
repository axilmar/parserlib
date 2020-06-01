#ifndef PARSERLIB__EXPRESSION_WRAPPER__HPP
#define PARSERLIB__EXPRESSION_WRAPPER__HPP


#include "expression_interface.hpp"


namespace parserlib
{


    /**
        Polymorphic expression wrapper.
        @param ParseContext type of parse context to use in parsing.
        @param T type of expression to wrap.
     */
    template <typename ParseContext, typename T>
    class expression_wrapper : public expression_interface<ParseContext>
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        expression_wrapper(T&& expression)
            : m_expression(std::move(expression))
        {
        }

        /**
            Parse interface implementation.
            @param pc parse context.
            @return parse result.
         */
        virtual parse_result parse(ParseContext& pc) const override
        {
            return m_expression.parse(pc);
        }

    private:
        //expression
        T m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_WRAPPER__HPP
