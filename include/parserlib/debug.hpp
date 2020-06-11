#ifndef PARSERLIB__DEBUG__HPP
#define PARSERLIB__DEBUG__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        
        A helper parser for debugging.
        @param T type of expression to debug.
     */
    template <typename T> 
    class debug : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
         */
        debug(const T& expression)
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


} //namespace parserlib


#endif //PARSERLIB__DEBUG__HPP
