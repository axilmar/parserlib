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

            //try to activate this rule at the current position
            const activation_result activation = activate(pc);

            switch (activation)
            {
                //first activation
                case activation_result::first:
                {
                    const bool was_left_recursion = pc.left_recursion;

                    //parse
                    result = m_expression->parse(pc);

                    //process left recursion from the root rule that it appeared from
                    if (result == parse_result::accepted && !was_left_recursion && pc.left_recursion)
                    {
                        //accept the parsing from left-recursive rules without actually parsing
                        pc.accept_left_recursion = true;

                        //loop until no more parsing can be done
                        for (bool loop = true; loop && pc.valid(); )
                        {
                            //reactivate the rule so as that it captures the new position
                            pc.rule_positions.find(this)->second.back() = pc.iterator;

                            //reparse
                            const parse_result lr_result = m_expression->parse(pc);

                            switch (lr_result)
                            {
                                //continue the loop
                                case parse_result::accepted:
                                    break;

                                //stop the loop
                                case parse_result::rejected:
                                    loop = false;
                                    break;

                                //left recursion should not happen here; abort parsing
                                case parse_result::left_recursion:
                                    loop = false;
                                    result = parse_result::left_recursion;
                                    break;
                            }
                        }

                        //restore the state
                        pc.left_recursion = false;
                        pc.accept_left_recursion = false;
                    }
                    break;
                }

                //next activation
                case activation_result::next:
                {
                    //store the current left recursion state in order to restore it later
                    const bool prev_left_recursion = pc.left_recursion;
                    const bool prev_accept_left_recursion = pc.accept_left_recursion;

                    //reset the left recursion state since the rule was successfully activated
                    pc.left_recursion = false;
                    pc.accept_left_recursion = false;

                    //parse
                    result = m_expression->parse(pc);

                    //restore the left recursion state
                    pc.left_recursion = prev_left_recursion;
                    pc.accept_left_recursion = prev_accept_left_recursion;
                    break;
                }

                //handle left recursion
                case activation_result::left_recursion:
                    if (!pc.accept_left_recursion)
                    {
                        pc.left_recursion = true;
                        result = parse_result::left_recursion;
                    }
                    else
                    {
                        result = parse_result::accepted;
                    }

                    break;
            }

            //deactivate the rule
            deactivate(pc);

            return result;
        }

    private:
        //the expression is wrapped by a polymorphic type.
        std::unique_ptr<expression_interface<ParseContext>> m_expression;

        //activation result
        enum class activation_result
        {
            first,
            next,
            left_recursion
        };

        //tries to activate the current rule at the current position;
        //a rule cannot be activated twice at the same position;
        //that case is left recursion
        activation_result activate(ParseContext& pc) const
        {
            auto it = pc.rule_positions.find(this);

            //if an entry is not found, this is the first rule activation
            if (it == pc.rule_positions.end())
            {
                pc.rule_positions.emplace(this, std::vector<typename ParseContext::input_type::const_iterator>{ pc.iterator });
                return activation_result::first;
            }

            //if the current position is not the same as the last position,
            //add one more entry
            if (it->second.back() != pc.iterator)
            {
                it->second.push_back(pc.iterator);
                return activation_result::next;
            }

            //left recursion found
            it->second.push_back(pc.iterator);
            return activation_result::left_recursion;
        }

        //deactivate the rule
        void deactivate(ParseContext& pc) const
        {
            auto it = pc.rule_positions.find(this);
            if (it != pc.rule_positions.end())
            {
                it->second.pop_back();
                if (it->second.empty())
                {
                    pc.rule_positions.erase(it);
                }
            }
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
