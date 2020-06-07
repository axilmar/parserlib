#ifndef PARSERLIB__MATCH__HPP
#define PARSERLIB__MATCH__HPP


#include <string_view>
#include "expression_type.hpp"


namespace parserlib
{


    /**
        A parser that adds a match if an expression parses successfully.
        @param T type of expression to add a match for.
    */
    template <typename T> 
    class match : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
            @param tag tag.
        */
        template <typename T>
        match(T&& expression, const std::string_view& tag)
            : m_expression(std::forward<T>(expression))
            , m_tag(tag)
        {
        }

        /**
            Parses the given expression.
            If the expression returns 'accepted', a match is added to the context.
            @param pc parse context.
            @return always 'accepted'.
        */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            const auto start_position = pc.start_position;

            parse_result result = m_expression.parse(pc);

            if (result == parse_result::accepted)
            { 
                pc.add_match(m_tag, start_position, pc.position);
            }

            return result;
        }

    private:
        T m_expression;
        std::string_view m_tag;
    };


    /**
        Operator that creates a match out of an exception.
        @param expression expression to create a match for.
        @param tag match tag.
        @return an match expression.
    */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    match<expression_type_t<T>> operator == (T&& expression, const std::string_view& tag)
    {
        return { expression_type_t<T>(std::forward<T>(expression)), tag };
    }


} //namespace parserlib


#endif //PARSERLIB__MATCH__HPP
