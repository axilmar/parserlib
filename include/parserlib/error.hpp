#ifndef PARSERLIB__ERROR__HPP
#define PARSERLIB__ERROR__HPP


#include <string_view>
#include "expression_type.hpp"


namespace parserlib
{


    template <typename T, typename Tag> class match;


    /**
        Parse error.
        @param ParseContext type of parse context.
     */
    template <typename ParseContext> 
    class parse_error : public std::runtime_error
    {
    public:
        typedef typename ParseContext::input_type::const_iterator input_iterator_type;

        /**
            Constructor.
            @param pc parse context.
         */
        parse_error(const ParseContext& pc)
            : std::runtime_error("parse error")
            , m_position(pc.position)
        {
        }

        /**
            Returns the input position.
            @return the input position.
         */
        input_iterator_type position() const
        {
            return m_position;
        }

        /**
            Returns the tag.
            A tag is set by the match object that encloses the error object.
            @return the tag.
         */
        const std::string_view& tag() const
        {
            return m_tag;
        }

    private:
        input_iterator_type m_position;
        std::string_view m_tag;

        template <typename T, typename Tag> friend class match;
    };


    /**
        A parser that throws an exception when parsing fails.
        @param T type of expression to add an error to.
    */
    template <typename T> 
    class error : public expression
    {
    public:
        /**
            Constructor.
            @param expression expression.
        */
        error(T&& expression)
            : m_expression(std::move(expression))
        {
        }

        /**
            Parses the given expression.
            If the expression returns 'rejected', an expression is thrown.
            @param pc parse context.
            @return result of expression.
        */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            parse_result result = m_expression.parse(pc);

            if (result == parse_result::rejected)
            {
                throw parse_error(pc);
            }

            return result;
        }

    private:
        T m_expression;
    };


    /**
        Operator that creates a error out of an exception.
        @param expression expression to create a error for.
        @return an error expression.
    */
    template <typename T, typename = std::enable_if_t<has_expression_type_v<T>>> 
    error<expression_type_t<T>> operator ~(T&& expression)
    {
        return expression_type_t<T>(std::forward<T>(expression));
    }


} //namespace parserlib


#endif //PARSERLIB__ERROR__HPP
