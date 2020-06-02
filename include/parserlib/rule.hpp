#ifndef PARSERLIB__RULE__HPP
#define PARSERLIB__RULE__HPP


#include <memory>
#include "rule_reference.hpp"
#include "parse_context.hpp"
#include "expression_wrapper.hpp"


namespace parserlib
{


    /**
        A grammar rule.
        @param ParseContext type of parse context to use.
     */
    template <typename ParseContext = parse_context<>>
    class rule : public expression_with_unary_operators<rule<ParseContext>>
    {
    public:
        /**
            Constructor from expression.
            @param expression expression.
         */
        template <typename T>
        rule(T&& expression)
            : m_expression(std::make_unique<expression_wrapper<ParseContext, expression_type_t<T>>>(std::forward<T>(expression)))
        {
        }

        /**
            Constructor from rule reference.
            @param r rule.
         */
        rule(rule&& r)
            : m_expression(std::make_unique<expression_wrapper<rule_reference<ParseContext>>>(r))
        {
        }

        /**
            Parses the input with the underlying expression.
            It handles left recursion.
            @param pc parse context.
            @return parse result.
         */
        parse_result parse(ParseContext& pc) const
        {
            parse_result result;

            //activate the rule and check if left recursion is found
            const bool is_left_recursive = activate(pc);

            switch (pc.left_recursion.state)
            {
                //not in left recursion phase
                case left_recursion_state::inactive:

                    //activate left recursion
                    if (is_left_recursive)
                    {
                        pc.left_recursion.state = left_recursion_state::reject;
                        result = parse_result::left_recursion;
                    }

                    //else continue parsing to find the left-most non-recursive rule that can parse the input
                    else
                    {
                        const auto prev_left_recursion = pc.left_recursion;

                        result = m_expression->parse(pc);

                        //start the 'accept' phase of left recursion
                        if (pc.left_recursion.state == left_recursion_state::reject)
                        {
                            if (result == parse_result::accepted)
                            {
                                pc.left_recursion.state = left_recursion_state::accept;

                                for (bool loop = true; loop && pc.valid(); )
                                {
                                    //setup this rule to immediately report 'accepted'
                                    pc.positions[this].back() = pc.iterator;

                                    //adjust the left recursion position
                                    //after something is parsed
                                    //in order to cause left-recursive rules
                                    //to return 'accepted'
                                    pc.left_recursion.position = pc.iterator;

                                    const parse_result lr_result = m_expression->parse(pc);

                                    switch (lr_result)
                                    {
                                        //continue left recursion parsing
                                        case parse_result::accepted:
                                            break;

                                        //stop parsing left recursion
                                        case parse_result::rejected:
                                            loop = false;
                                            break;

                                        //abort left recursion
                                        case parse_result::left_recursion:
                                            result = parse_result::left_recursion;
                                            loop = false;
                                            break;
                                    }
                                }
                            }

                            pc.left_recursion = prev_left_recursion;
                        }
                    }

                    break;

                //reject phase
                case left_recursion_state::reject:
                    
                    //left recursion found
                    if (is_left_recursive)
                    {
                        result = parse_result::left_recursion;
                    }

                    //else continue parsing to find the left-most non-recursive rule that can parse the input
                    else
                    {
                        result = m_expression->parse(pc);
                    }
                    
                    break;

                //accept phase
                case left_recursion_state::accept:

                    //if left recursion is found, return 'accept'
                    //in order to proceed with the next part of the grammar
                    if (is_left_recursive)
                    {
                        result = parse_result::accepted;
                    }

                    //else if there was no progress since the accept phase started,
                    //continue parsing the subrules
                    else if (pc.iterator == pc.left_recursion.position)
                    {
                        result = m_expression->parse(pc);
                    }

                    //else there was some progress, so reset the current
                    //left recursion state temporarily
                    else
                    {
                        const auto prev_left_recursion = pc.left_recursion;
                        pc.left_recursion = { left_recursion_state::inactive, pc.iterator };
                        result = m_expression->parse(pc);
                        pc.left_recursion = prev_left_recursion;
                    }
                    break;
            }

            //remove the rule position
            deactivate(pc);

            return result;
        }

    private:
        //the expression is wrapped by a polymorphic type.
        std::unique_ptr<expression_interface<ParseContext>> m_expression;

        //activate; returns true on left recursion.
        bool activate(ParseContext& pc) const
        {
            auto& positions = pc.positions[this];
            positions.push_back(pc.iterator);
            return positions.size() >= 2 && positions[positions.size() - 1] == positions[positions.size() - 2];
        }

        //deactivate.
        void deactivate(ParseContext& pc) const
        {
            pc.positions.find(this)->second.pop_back();
        }
    };


    /**
        Specialization for rules.
        Allows the implicit conversion to rules to rule references.
        @param ParseContext type of parse context to use.
     */
    template <typename ParseContext>
    class expression_type<rule<ParseContext>>
    {
    public:
        /**
            The expression type of a rule is a rule reference,
            in order to allow recursive declarations of rules.
         */
        typedef rule_reference<ParseContext> type;
    };


} //namespace parserlib


#endif //PARSERLIB__RULE__HPP
