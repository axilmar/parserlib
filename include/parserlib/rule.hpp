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

            return result;
        }

    private:
        //the expression is wrapped by a polymorphic type.
        std::unique_ptr<expression_interface<ParseContext>> m_expression;

        //activate
        bool activate(ParseContext& pc) const
        {
            const auto it = pc.positions.find(this);
            
            if (it == pc.positions.end())
            {
                pc.positions.emplace(this, std::vector<typename ParseContext::input_type::const_iterator>({pc.iterator}));
                return true;
            }

            if (pc.iterator > it->second.back())
            {
                it->second.push_back(pc.iterator);
                return true;
            }

            return false;
        }

        //deactivate
        void deactivate(ParseContext& pc) const
        {
            const auto it = pc.positions.find(this);

            if (it != pc.positions.end())
            {
                it->second.pop_back();

                if (it->second.empty())
                {
                    pc.positions.erase(it);
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
