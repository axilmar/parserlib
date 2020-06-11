#ifndef PARSERLIB__ERROR__HPP
#define PARSERLIB__ERROR__HPP


#include <any>
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
        ///input iterator type.
        typedef typename ParseContext::input_type::const_iterator input_iterator_type;

        ///tag type.
        typedef std::any tag_type;

        /**
            Constructor.
            @param pc parse context.
            @param error_position_start error position start.
            @param error_position_end error position end.
         */
        parse_error(
            ParseContext& pc, 
            const input_iterator_type error_position_start, 
            const input_iterator_type error_position_end)
            : std::runtime_error("parse error")
            , m_parse_context(&pc)
            , m_error_position_start(error_position_start)
            , m_error_position_end(error_position_end)
        {
        }

        /**
            Returns the parse context.
            @return the parse context.
         */
        ParseContext& parse_context() const
        {
            return *m_parse_context;
        }

        /**
            Returns the error position start.
            @return the error position start.
         */
        input_iterator_type error_position_start() const
        {
            return m_error_position_start;
        }

        /**
            Returns the error position end.
            @return the error position end.
         */
        input_iterator_type error_position_end() const
        {
            return m_error_position_end;
        }

        /**
            Returns the tag.
            A tag is set by the match object that encloses the error object.
            @return the tag.
         */
        const tag_type& tag() const
        {
            return m_tag;
        }

    private:
        ParseContext* m_parse_context;
        input_iterator_type m_error_position_start;
        input_iterator_type m_error_position_end;
        tag_type m_tag;

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
            const auto start_position = pc.position;
            parse_result result = m_expression.parse(pc);
            if (result == parse_result::accepted)
            {
                return result;
            }
            throw parse_error(pc, start_position, pc.furthest_position);
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
