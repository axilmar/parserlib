#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <functional>
#include "RuleExpression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "ParseContext.hpp"
#include "RuleReference.hpp"
#include "ASTNode.hpp"


namespace parserlib
{


    /**
        Class that represents a production rule.
        @param ParseContextType parse context.
     */
    template <typename ParseContextType = ParseContext<>> class Rule :
        public RuleExpression,
        public UnaryOperatorsBase<Rule<ParseContextType>>
    {
    public:
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
            m_expression(
                [expr = std::move(ExpressionTypeT<ExpressionType>(expression))](ParseContextType& pc)
                {
                    return expr.parse(pc);
                })
        {
        }

        /**
            Constructor.
            @param rule rule reference.
         */
        Rule(Rule& rule) :
            m_expression(
                [ruleRef = RuleReference<ParseContextType>(rule)](ParseContextType& pc)
                {
                    return ruleRef.parse(pc);
                })
        {
        }

        /**
            Constructor.
            @param name name.
            @param expr expression.
         */
        template <typename ExpressionType>
        Rule(const std::string& name, ExpressionType&& expression) :
            RuleExpression(name),
            m_expression(
                [expr = std::move(ExpressionTypeT<ExpressionType>(expression))](ParseContextType& pc)
                {
                    return expr.parse(pc);
                })
        {
        }

        /**
            Constructor.
            @param name name.
            @param rule rule reference.
         */
        Rule(const std::string& name, Rule& rule) :
            RuleExpression(name),
            m_expression(
                [ruleRef = RuleReference<ParseContextType>(rule)](ParseContextType& pc)
                {
                    return ruleRef.parse(pc);
                })
        {
        }

        /**
            Parses the input with the given rule.
            @param pc parse context.
            @return true on success, false on failure.
         */
        bool parse(ParseContextType& pc)
        {
            bool result{};

            switch (m_state)
            {
                case START:
                    {
                        auto prevLastInput = m_lastInput;
                        m_state = PARSE;
                        m_lastInput = pc.getCurrentPosition();
                        const auto startInput = m_lastInput;

                        try
                        {
                            result = _parse(pc, startInput);
                        }

                        catch (LeftRecursion)
                        {
                            m_state = REJECT;
                            try
                            {
                                result = _parse(pc, startInput);
                            }
                            catch (LeftRecursion)
                            {
                                m_state = START;
                                m_lastInput = prevLastInput;
                                throw;
                            }

                            if (result)
                            {
                                m_state = ACCEPT;
                                while (pc.isValidPosition())
                                {
                                    m_lastInput = pc.getCurrentPosition();
                                    try
                                    {
                                        if (!_parse(pc, startInput))
                                        {
                                            break;
                                        }
                                    }
                                    catch (LeftRecursion)
                                    {
                                        m_state = START;
                                        m_lastInput = prevLastInput;
                                        throw;
                                    }
                                }
                            }
                        }

                        m_state = START;
                        m_lastInput = prevLastInput;
                    }
                    break;

                case PARSE:
                    if (pc.getCurrentPosition() > m_lastInput)
                    {
                        auto prevLastInput = m_lastInput;
                        m_state = START;
                        result = parse(pc);
                        m_state = PARSE;
                        m_lastInput = prevLastInput;
                    }
                    else
                    {
                        throw LeftRecursion();
                    }
                    break;

                case REJECT:
                    if (pc.getCurrentPosition() > m_lastInput)
                    {
                        auto prevLastInput = m_lastInput;
                        m_state = START;
                        result = parse(pc);
                        m_state = REJECT;
                        m_lastInput = prevLastInput;
                    }
                    else
                    {
                        result = false;
                    }
                    break;

                case ACCEPT:
                    if (pc.getCurrentPosition() > m_lastInput)
                    {
                        auto prevLastInput = m_lastInput;
                        m_state = START;
                        result = parse(pc);
                        m_state = ACCEPT;
                        m_lastInput = prevLastInput;
                    }
                    else
                    {
                        result = true;
                    }
                    break;
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
        //states
        enum State
        {
            START,
            PARSE,
            REJECT,
            ACCEPT
        };

        //used for catching left recursion through non-local goto
        struct LeftRecursion
        {
        };

        //current state
        State m_state = START;

        //last known position
        typename ParseContextType::IteratorType m_lastInput;

        //grammar
        std::function<bool(ParseContextType&)> m_expression;

        //callback
        CallbackType m_callback = [](const MatchType &, ASTNodeStack&) {};

        //internal parse; if successful, a match is added
        bool _parse(ParseContextType& pc, typename ParseContextType::IteratorType startPosition)
        {
            if (m_expression(pc))
            {
                pc.addMatch(*this, startPosition, pc.getCurrentPosition());
                return true;
            }
            return false;
        }
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
