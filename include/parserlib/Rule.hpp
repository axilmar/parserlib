#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include "UnaryOperatorsBase.hpp"
#include "ParseContext.hpp"
#include "RuleReference.hpp"
#include "ASTNode.hpp"
#include "ExpressionWrapper.hpp"
#include "ScopedValueChange.hpp"
#include "LeftRecursion.hpp"


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

        ///position index type
        typedef typename ParseContextType::PositionIndexType PositionIndexType;

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
            Parses the input with the current rule.
            @param pc parse context.
            @return true on success, false on failure.
         */
        bool parse(ParseContextType& pc)
        {
            try
            {
                return _parseLR(pc);
            }
            catch (LeftRecursionEndedSuccessfully)
            {
                return true;
            }
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
        CallbackType m_callback;

        //last known position
        PositionIndexType m_lastKnownPositionIndex = 0;

        //left recursion state
        enum LR_State
        {
            LR_Init,
            LR_Reject,
            LR_Accept
        } m_LR_State = LR_Init;

        //internal parse which adds a match
        bool _parse(ParseContextType& pc)
        {
            const auto startPosition = pc.getCurrentPosition();
            const auto v = scopedValueChange(m_lastKnownPositionIndex, [&]() { return &(m_lastKnownPositionIndex = pc.getCurrentPositionIndex()); });

            try
            {
                if (!m_expression->parse(pc))
                {
                    return false;
                }
            }
            catch (LeftRecursionEndedSuccessfully)
            {
            }

            if (m_callback)
            {
                pc.addMatch(this, startPosition, pc.getCurrentPosition(), m_callback);
            }
            return true;
        }

        //Parses the input with the current rule, taking into account the left recursion.
        bool _parseLR(ParseContextType& pc)
        {
            bool result;

            //if left recursion
            if (pc.getCurrentPositionIndex() == m_lastKnownPositionIndex)
            {
                switch (m_LR_State)
                {
                    case LR_Init:
                    {
                        const auto v = scopedValueChange(m_LR_State, [&]() { return &(m_LR_State = LR_Reject); });
                        result = _parse(pc);
                        if (result)
                        {
                            m_LR_State = LR_Accept;
                            while (pc.isValidPosition() && _parse(pc))
                            {
                            }
                            throw LeftRecursionEndedSuccessfully();
                        }
                        break;
                    }

                    case LR_Reject:
                        result = false;
                        break;

                    case LR_Accept:
                        result = true;
                        break;
                }
            }

            //else no left recursion
            else
            {
                const auto v = scopedValueChange(m_LR_State, [&]() { return &(m_LR_State = LR_Init); });
                result = _parse(pc);
            }

            return result;
        }

        friend class RuleReference<ParseContextType>;
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
