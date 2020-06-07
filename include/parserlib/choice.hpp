#ifndef PARSERLIB__CHOICE__HPP
#define PARSERLIB__CHOICE__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**
        A parser that represents a choice of parsers.
        @param L left side expression.
        @param R right side expression.
     */
    template <typename L, typename R> 
    class choice : public expression
    {
    public:
        /**
            Constructor.
            @param left_expression left side expression.
            @param right_expression right side expression.
         */
        template <typename L, typename R>
        choice(L&& left_expression, R&& right_expression)
            : m_left_expression(std::forward<L>(left_expression))
            , m_right_expression(std::forward<R>(right_expression))
        {
        }

        /**
            Parses the given item.
            For it to be successful, either the left or right expression must be successful.
            @param pc parse context.
            @return parse result.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            const auto start_state = pc.state();

            parse_result result = m_left_expression.parse(pc);

            switch (result)
            {
                //left expression success
                case parse_result::accepted:
                    break;

                //left expression failure; try the right expression
                case parse_result::rejected:
                    pc.set_state(start_state);
                    
                    result = m_right_expression.parse(pc);

                    switch (result)
                    {
                        //right expression success
                        case parse_result::accepted:
                            if (pc.m_left_recursion_state == ParseContext::left_recursion_state::reject)
                            {
                                while (pc.valid())
                                {
                                    pc.m_left_recursion_state = ParseContext::left_recursion_state::accept;
                                    pc.m_left_recursion_position = pc.position;

                                    const parse_result result = m_left_expression.parse(pc);
                                        
                                    if (result == parse_result::rejected)
                                    {
                                        break;
                                    }
                                }

                                pc.m_left_recursion_state = ParseContext::left_recursion_state::reject;
                            }
                            break;

                            //right expression failure; rewind the state
                        case parse_result::rejected:
                            pc.set_state(start_state);
                            break;
                    }
                    break;
            }

            return result;
        }

    private:
        //left and right side expressions
        L m_left_expression;
        R m_right_expression;
    };


    /**
        Choice operator.
        It is enabled only if types L and R both have expression types.
        @param left_expression left side expression.
        @param right_expression right side expression.
        @return a choice object that contains both expressions.
     */
    template <typename L, typename R, typename = std::enable_if_t<have_expression_types_v<L, R>>>
    choice<expression_type_t<L>, expression_type_t<R>> 
        operator | (L&& left_expression, R&& right_expression)
    {
        return
        { 
            expression_type_t<L>(std::forward<L>(left_expression)), 
            expression_type_t<R>(std::forward<R>(right_expression)), 
        };
    }


} //namespace parserlib


#endif //PARSERLIB__CHOICE__HPP
