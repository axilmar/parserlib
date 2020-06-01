#ifndef PARSERLIB__EXPRESSION_WRAPPER__HPP
#define PARSERLIB__EXPRESSION_WRAPPER__HPP


#include "expression_interface.hpp"


namespace parserlib
{


    /**
        Polymorphic expression wrapper.
        @param T type of expression to wrap.
     */
    template <typename T>
    class expression_wrapper : public expression_interface
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

    private:
        //expression
        T m_expression;
    };


} //namespace parserlib


#endif //PARSERLIB__EXPRESSION_WRAPPER__HPP
