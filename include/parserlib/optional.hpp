#ifndef PARSERLIB__OPTIONAL__HPP
#define PARSERLIB__OPTIONAL__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        
        A parser that makes an expression optional.
        @param T type of expression to make optional.
     */
    template <typename T> 
    class optional : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        template <typename T>
        optional(T&& expression)
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

            switch (result)
            {
                case parse_result::accepted:
                case parse_result::accepted_left_recursion:
                    break;

                case parse_result::rejected:
                case parse_result::rejected_left_recursion:
                    pc.set_state(start_state);
                    result = parse_result::accepted;
                    break;
            }

            return result;
        }

    private:
        T m_expression;
    };


    /**
        Operator that makes an expression optional.
        @param expression expression to make optional.
        @return an optional expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    optional<expression_type_t<T>> operator - (T&& expression)
    {
        return expression_type_t<T>(std::forward<T>(expression));
    }


} //namespace parserlib


#endif //PARSERLIB__OPTIONAL__HPP
