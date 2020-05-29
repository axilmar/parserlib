#ifndef PARSERLIB_MATCHEXPRESSION_HPP
#define PARSERLIB_MATCHEXPRESSION_HPP


#include <type_traits>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "LeftRecursion.hpp"


namespace parserlib
{


    /**
        Helper class used for for matching individual expressions.
        It allows adding individual expressions to the match output
        without needing them to be assigned to rules.
        @param ExpressionType type of expression to match.
        @param ASTNodeType type of AST object to create from this expression.
     */
    template <typename ExpressionType, class ASTNodeType> class MatchExpression : 
        public Expression,
        public UnaryOperatorsBase<MatchExpression<ExpressionType, ASTNodeType>>
    {
    public:
        /**
            Constructor.
            @param expression expression to match.
         */
        MatchExpression(const ExpressionType& expression)
            : m_expression(expression)
        {
        }

        /**
            The parse function.
            If the sub-expression parses correctly,
            a match is added to the parse context.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            const auto startPosition = pc.getCurrentPosition();

            const auto addMatch = [&]()
            {
                pc.addMatch(
                    this, 
                    startPosition, 
                    pc.getCurrentPosition(), 
                    [](const typename ParseContextType::MatchType &match, ASTNodeStack& asn)
                {
                    asn.push_back(std::make_shared<ASTNodeType>(match, asn));
                });
            };

            try
            {
                if (!m_expression.parse(pc))
                {
                    return false;
                }
            }
            catch (LeftRecursionEndedSuccessfully)
            {
                addMatch();
                throw;
            }

            addMatch();
            return true;
        }

    private:
        //expression to parse
        const ExpressionType m_expression;
    };


    /**
        A simple class for declaring an AST type.
        @param Type AST type.
     */
    template <typename Type> class ASTType
    {
    public:
        ///type.
        typedef Type Type;
    };


    /**        
        Operator that allows the inline declaration of an AST node.
        @param expression expression to match.
        @param ast ast node type to create when this expression is matched.
        @return the match expression.
     */
    template <
        typename ExpressionType, 
        typename ASTNodeType,
        typename = std::enable_if_t<std::is_base_of_v<Expression, std::decay_t<ExpressionType>>>>
    MatchExpression<ExpressionTypeT<std::decay_t<ExpressionType>>, ASTNodeType> 
    operator == (ExpressionType&& expression, const ASTType<ASTNodeType>& ast)
    {
        return { ExpressionTypeT<std::decay_t<ExpressionType>>(expression) };
    }


    /**
        Helper function for allowing declaration of an AST class.
        @return an AST node type declaration.
     */
    template <typename ASTNodeType> ASTType<ASTNodeType> ast()
    {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_MATCHEXPRESSION_HPP