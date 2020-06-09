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
        optional(T&& expression)
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
            
            if (result == parse_result::rejected)
            { 
                pc.set_state(start_state);
                result = parse_result::accepted;
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
