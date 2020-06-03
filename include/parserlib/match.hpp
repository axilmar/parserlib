#ifndef PARSERLIB__MATCH__HPP
#define PARSERLIB__MATCH__HPP


#include <string_view>
#include "expression_type.hpp"


namespace parserlib
{


    /**
        Class that represents a match.
        @param T type of expression to match.
     */
    template <typename T>
    class match : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
            @param tag match tag.
         */
        template <typename T>
        match(T&& expression, const std::string_view& tag)
            : m_expression(std::forward<T>(expression))
            , m_tag(tag)
        {
        }

        /**
            Parses the expression, and adds a match on success.
            @param pc parse context.
            @return parse result.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            const auto start_position = pc.position;
            const parse_result result = m_expression.parse(pc);
            if (result == parse_result::accepted)
            {
                pc.add_match(start_position, pc.position, m_tag);
            }
            return result;
        }

    private:
        T m_expression;
        std::string_view m_tag;
        mutable size_t m_recursion_count = 0;
    };


    /**
        Operator that creates a match out of an expression.
        @param expression expression to create a match out of.
        @param tag tag to add to the match.
        @return the match expression.
     */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>>
    match<expression_type_t<T>> operator >= (T&& expression, const std::string_view& tag)
    {
        return { expression_type_t<T>(std::forward<T>(expression)), tag };
    }


} //namespace parserlib


#endif //PARSERLIB__MATCH__HPP
