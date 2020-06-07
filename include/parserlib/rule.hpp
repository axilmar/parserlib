#ifndef PARSERLIB__RULE__HPP
#define PARSERLIB__RULE__HPP


#include <memory>
#include <string_view>
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
    class rule : public expression
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
            If the tag is not empty, and parsing is successful,
            then the corresponding match is added to the parse context.
            @param pc parse context.
            @return parse result.
         */
        parse_result parse(ParseContext& pc) const
        {
            parse_result result;

            const bool left_recursion = pc.add_position(this);

            if (!left_recursion)
            {
                if (pc.position > pc.m_left_recursion_position)
                {
                    const auto prev_start_position = pc.start_position;
                    const auto prev_left_recursion_state = pc.m_left_recursion_state;
                    const auto prev_left_recursion_position = pc.m_left_recursion_position;

                    pc.start_position = pc.position;
                    pc.m_left_recursion_state = ParseContext::left_recursion_state::inactive;
                    pc.m_left_recursion_position = pc.position;

                    result = parse_(pc);

                    pc.start_position = prev_start_position;
                    pc.m_left_recursion_state = prev_left_recursion_state;
                    pc.m_left_recursion_position = prev_left_recursion_position;
                }
                else
                {
                    switch (pc.m_left_recursion_state)
                    {
                        case ParseContext::left_recursion_state::inactive:
                            result = parse_(pc);
                            break;

                        case ParseContext::left_recursion_state::reject:
                            result = parse_(pc);
                            break;

                        case ParseContext::left_recursion_state::accept:
                            result = parse_result::accepted;
                            break;
                    }
                }
            }

            else
            {
                switch (pc.m_left_recursion_state)
                {
                    case ParseContext::left_recursion_state::inactive:
                        pc.m_left_recursion_state = ParseContext::left_recursion_state::reject;
                        result = parse_result::rejected;
                        break;

                    case ParseContext::left_recursion_state::reject:
                        result = parse_result::rejected;
                        break;

                    case ParseContext::left_recursion_state::accept:
                        result = parse_result::accepted;
                        break;
                }
            }

            pc.remove_position(this);

            return result;
        }

    private:
        //the expression is wrapped by a polymorphic type.
        std::unique_ptr<expression_interface<ParseContext>> m_expression;

        //internal parse that adds match on success
        parse_result parse_(ParseContext& pc) const
        {
            const auto start_position = pc.start_position;
            
			const parse_result result = m_expression->parse(pc);
            
			if (result == parse_result::accepted)
            {
                pc.add_match(this, start_position, pc.position);
            }
            
			return result;
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


    /**
        Operator that adds a tag to a rule.
        @param rule rule to set the tag of.
        @param tag tag to set.
        @return the rule.
     */
    template <typename ParseContext>
    rule<ParseContext>& operator >= (rule<ParseContext>& rule, const std::string_view& tag)
    {
        rule.tag = tag;
        return rule;
    }


    /**
        Generic parse function.
        @param grammar root rule of grammar to use.
        @param pc parse context.
        #return true if parsing succeeded (i.e. if the whole input was accepted), otherwise false.
     */
    template <typename ParseContext> 
    bool parse(rule<ParseContext>& grammar, ParseContext& pc)
    {
        const parse_result result = grammar.parse(pc);
        return result == parse_result::accepted && pc.remaining_input().empty();
    }


} //namespace parserlib


#endif //PARSERLIB__RULE__HPP
