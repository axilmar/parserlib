#ifndef PARSERLIB__LOOP__HPP
#define PARSERLIB__LOOP__HPP


#include "expression_type.hpp"


namespace parserlib
{


    template <typename L, typename R> class sequence;


    /**
        A parser that invokes an expression zero or more times.
        @param T type of expression to make a loop out of.
     */
    template <typename T>
    class loop : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        template <typename T>
        loop(T&& expression)
            : m_expression(std::forward<T>(expression))
        {
        }

        /**
            Parses the given expression.
            @param pc parse context.
            @returns always 'accepted'.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            while (pc.valid())
            {
                const auto start_state = pc.state();

                const parse_result result = m_expression.parse(pc);
                
                if (result == parse_result::rejected)
                { 
                    pc.set_state(start_state);
                    break;
                }
            }

            return parse_result::accepted;
        }

    private:
        T m_expression;
    };


    /**
        Operator that makes a loop out of an expression.
        @param expression expression to make a loop out of.
        @return a loop expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    loop<expression_type_t<T>> operator * (T&& expression)
    {
        return expression_type_t<T>(std::forward<T>(expression));
    }


    /**
        Operator that makes a loop out of an expression that must be parsed at least once.
        @param expression expression to make a loop out of.
        @return a loop expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    sequence<expression_type_t<T>, loop<expression_type_t<T>>> operator + (T&& expression)
    {
        return
        { 
            expression_type_t<T>(std::forward<T>(expression)), 
            expression_type_t<T>(std::forward<T>(expression)) 
        };
    }


} //namespace parserlib


#endif //PARSERLIB__LOOP__HPP
