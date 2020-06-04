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
        ///match tag.
        std::string_view tag;

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
            return m_expression->parse(pc);
        }

    private:
        //the expression is wrapped by a polymorphic type.
        std::unique_ptr<expression_interface<ParseContext>> m_expression;

        //internal parse that adds match on success
        parse_result parse_(ParseContext& pc) const
        {
            const auto start_position = pc.position;
            const parse_result result = m_expression->parse(pc);
            if (result == parse_result::accepted && !tag.empty())
            {
                pc.add_match(start_position, pc.position, tag);
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


} //namespace parserlib


#endif //PARSERLIB__RULE__HPP
