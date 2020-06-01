#ifndef PARSERLIB__SEQUENCE__HPP
#define PARSERLIB__SEQUENCE__HPP


#include "expression_with_unary_operators.hpp"


namespace parserlib
{


    /**
        An expression that represents a sequence of parsers.
        @param L left side expression.
        @param R right side expression.
     */
    template <typename L, typename R> 
    class sequence : public expression_with_unary_operators<sequence<L, R>>
    {
    public:
        /**
            Constructor.
            @param left_expression left side expression.
            @param right_expression right side expression.
         */
        sequence(L&& left_expression, R&& right_expression)
            : m_left_expression(std::move(left_expression))
            , m_right_expression(std::move(right_expression))
        {
        }

    private:
        //left and right side expressions
        L m_left_expression;
        R m_right_expression;
    };


    /**
        Sequence operator.
        It is enabled only if types L and R both have expression types.
        @param left_expression left side expression.
        @param right_expression right side expression.
        @return a sequence object that contains both expressions.
     */
    template <typename L, typename R, typename = std::enable_if_t<have_expression_types_v<L, R>>>
    sequence<expression_type_t<L>, expression_type_t<R>> 
        operator >> (L&& left_expression, R&& right_expression)
    {
        return
        { 
            expression_type_t<L>(std::forward<L>(left_expression)), 
            expression_type_t<R>(std::forward<R>(right_expression)), 
        };
    }


} //namespace parserlib


#endif //PARSERLIB__SEQUENCE__HPP
