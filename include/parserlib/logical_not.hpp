#ifndef PARSERLIB__LOGICAL_NOT__HPP
#define PARSERLIB__LOGICAL_NOT__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        
        A parser that parses input, inverts the result, but does not consume any of it.
        @param T type of expression to wrap into a logical not.
     */
    template <typename T> 
    class logical_not : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        template <typename T>
        logical_not(T&& expression)
            : m_expression(std::forward<T>(expression))
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
            const auto start_state = pc.get_state();
            parse_result result = m_expression.parse(pc);
            pc.set_state(start_state);
            switch (result)
            {
                case parse_result::accepted:
                    result = parse_result::rejected;
                    break;

                case parse_result::rejected:
                    result = parse_result::accepted;
                    break;
            }
            return result;
        }

    private:
        T m_expression;
    };


    /**
        Operator that makes an expression logical_not.
        @param expression expression to make logical_not.
        @return an logical_not expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    logical_not<expression_type_t<T>> operator ! (T&& expression)
    {
        return expression_type_t<T>(std::forward<T>(expression));
    }


} //namespace parserlib


#endif //PARSERLIB__LOGICAL_NOT__HPP
