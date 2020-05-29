#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "UnaryOperatorsBase.hpp"
#include "ParseContext.hpp"
#include "RuleReference.hpp"
#include "ASTNode.hpp"
#include "ExpressionWrapper.hpp"


namespace parserlib
{


    /**
        Class that represents a production rule.
        @param ParseContextType parse context.
     */
    template <typename ParseContextType = ParseContext<>> class Rule :
        public Expression,
        public UnaryOperatorsBase<Rule<ParseContextType>>
    {
    public:
        ///iterator type.
        typedef typename ParseContextType::IteratorType IteratorType;

        ///match type.
        typedef typename ParseContextType::MatchType MatchType;

        ///Type of callback function to invoke when this rule is parsed.
        typedef std::function<void(const MatchType &, ASTNodeStack&)> CallbackType;

        /**
            Constructor.
            @param expr expression.
         */
        template <typename ExpressionType>
        Rule(ExpressionType&& expression) :
            m_expression(std::make_unique<ExpressionWrapper<ParseContextType, ExpressionTypeT<ExpressionType>>>(std::forward<ExpressionType>(expression)))
        {
        }

        /**
            Constructor.
            It creates a rule reference.
            @param rule rule reference.
         */
        Rule(Rule& rule) :
            m_expression(std::make_unique<ExpressionWrapper<ParseContextType, RuleReference<ParseContextType>>>(rule))
        {
        }

        /**
            Parses the input with the given rule.
            If the rule is matched, it adds a match to the current context.
            @param pc parse context.
            @return true on success, false on failure.
         */
        bool parse(ParseContextType& pc) const
        {
            const auto startPosition = pc.getCurrentPosition();
            const bool result = m_expression->parse(pc);
            if (result)
            {
                pc.addMatch(this, startPosition, pc.getCurrentPosition(), m_callback);
            }
            return result;
        }

        /**
            Returns the callback.
            @return the callback.
         */
        const CallbackType& getCallback() const
        {
            return m_callback;
        }

        /**
            Sets the callback.
            @param cb callback.
         */
        void setCallback(const CallbackType& cb)
        {
            m_callback = cb;
        }

    private:
        //grammar
        std::unique_ptr<IExpression<ParseContextType>> m_expression;

        //callback
        CallbackType m_callback = [](const MatchType &, ASTNodeStack&) {};
    };


    /**
        Specialization of expression type for Rule.
        It yields an object of type RuleReference<ParseContext>,
        in order to allow recursion in declarative style.
        @param ParseContextType parse context type.
     */
    template <typename ParseContextType> class ExpressionType<Rule<ParseContextType>>
    {
    public:
        ///type of expression for rule is rule reference; it enables recursion in declarative style.
        typedef RuleReference<ParseContextType> Type;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
