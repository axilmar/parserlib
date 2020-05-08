#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <string>
#include "Expression.hpp"
#include "ASTNode.hpp"


namespace parserlib
{


    /**
        A parser match.
        @param InputType type of input.
     */
    template <typename InputType = std::string> class Match
    {
    public:
        ///type of input iterator.
        typedef typename InputType::const_iterator IteratorType;

        ///type of function that creates an AST node.
        typedef std::function<void(const Match &, ASTNodeStack&)> CreateASTFunction;

        /**
            Constructor.
         */
        Match() :
            m_expression(nullptr)
        {
        }

        /**
            Constructor.
            @param expression expression that was matched.
            @param start input start.
            @param end input end.
         */
        template <typename CreateASTFunctionType>
        Match(
            const Expression* expression, 
            IteratorType start, 
            IteratorType end, 
            CreateASTFunctionType&& createASTFunction) :
            m_expression(expression),
            m_start(start),
            m_end(end),
            m_createASTFunction(std::forward<CreateASTFunctionType>(createASTFunction))
        {
        }

        /**
            Returns the expression that was matched.
            @return the expression that was matched.
         */
        const Expression* getExpression() const
        {
            return m_expression;
        }

        /**
            Returns the start of the input part that was matched.
            @return the start of the input part that was matched.
         */
        IteratorType getStart() const
        {
            return m_start;
        }

        /**
            Returns the end of the input part that was matched.
            @return the end of the input part that was matched.
         */
        IteratorType getEnd() const
        {
            return m_end;
        }

        /**
            Conversion to string.
            @return string that represents the match.
         */
        operator std::string () const
        {
            return std::string(m_start, m_end);
        }

        /**
            Returns the function that, when invoked,
            will create the AST node from this match.
            @return the create AST function.
         */
        const CreateASTFunction& getCreateASTFunction() const
        {
            return m_createASTFunction;
        }

    private:
        const Expression* m_expression;
        IteratorType m_start;
        IteratorType m_end;
        CreateASTFunction m_createASTFunction;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
