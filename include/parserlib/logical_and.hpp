#ifndef PARSERLIB__LOGICAL_AND__HPP
#define PARSERLIB__LOGICAL_AND__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        
        A parser that parses input, returns success/failure, but does not consume any of it.
        @param T type of expression to wrap into a logical and.
     */
    template <typename T> 
    class logical_and : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        logical_and(T&& expression)
            : m_expression(std::move(expression))
        {
        }

        /**
            Parses the given expression.
            @param pc parse context.
            @return always 'accepted'.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            const auto start_state = pc.state();
            parse_result result = m_expression.parse(pc);
            pc.set_state(start_state);
            return result;
        }

    private:
        T m_expression;
    };


    /**
        Operator that makes an expression logical_and.
        @param expression expression to make logical_and.
        @return an logical_and expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    logical_and<expression_type_t<T>> operator & (T&& expression)
    {
        return expression_type_t<T>(std::forward<T>(expression));
    }


} //namespace parserlib


#endif //PARSERLIB__LOGICAL_AND__HPP
