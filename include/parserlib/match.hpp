#ifndef PARSERLIB__MATCH__HPP
#define PARSERLIB__MATCH__HPP


#include "error.hpp"
#include "terminal.hpp"


namespace parserlib
{


    /**
        A parser that adds a match if an expression parses successfully.
        @param T type of expression to add a match for.
        @param Tag tag type.
    */
    template <typename T, typename Tag> 
    class match : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
            @param tag tag.
        */
        match(T&& expression, Tag&& tag)
            : m_expression(std::move(expression))
            , m_tag(std::move(tag))
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
            const auto start_position = pc.position;

            try
            {
                parse_result result = m_expression.parse(pc);

                if (result == parse_result::accepted)
                {
                    pc.add_match(m_tag, start_position, pc.position);
                }

                return result;
            }
            catch (parse_error<ParseContext>& pe)
            {
                if (pe.m_tag.empty())
                {
                    pe.m_tag = m_tag;
                }
                throw;
            }
        }

    private:
        T m_expression;
        Tag m_tag;
    };


    /**
        Operator that creates a match out of an exception.
        @param expression expression to create a match for.
        @param tag match tag.
        @return an match expression.
    */
    template <typename T, typename Tag, typename = std::enable_if_t<has_expression_type_v<T>>> 
    match<expression_type_t<T>, terminal_value_t<Tag>> operator == (T&& expression, Tag&& tag)
    {
        return
        {
            expression_type_t<T>(std::forward<T>(expression)), 
            terminal_value_t<Tag>(std::forward<Tag>(tag))
        };
    }


} //namespace parserlib


#endif //PARSERLIB__MATCH__HPP
