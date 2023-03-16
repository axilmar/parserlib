#ifndef PARSERLIB__DEBUG__HPP
#define PARSERLIB__DEBUG__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**        
        A helper parser for debugging an inline expression.
        @param T type of expression to debug.
     */
    template <typename T> 
    class debug_val : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        debug_val(const T& expression)
            : m_expression(expression)
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
            parse_result result = m_expression.parse(pc);
            return result;
        }

    private:
        T m_expression;
    };


    /**
        A helper parser for debugging an expression passed by reference.
        @param T type of expression to debug.
     */
    template <typename T>
    class debug_ref : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        debug_ref(T& expression)
            : m_expression(expression)
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
            parse_result result = m_expression.parse(pc);
            return result;
        }

    private:
        T& m_expression;
    };


    /**
     * Debugs an inline expression.
     */
    template <class T> debug_val<T> debug(const T& expr) {
        return {expr};
    }


    /**
     * Debugs an expression by reference.
     */
    template <class T> debug_ref<T> debug(T& expr) {
        return { expr };
    }


} //namespace parserlib


#endif //PARSERLIB__DEBUG__HPP
