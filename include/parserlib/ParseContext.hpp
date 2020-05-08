#ifndef PARSERLIB_PARSECONTEXT_HPP
#define PARSERLIB_PARSECONTEXT_HPP


#include <string>
#include <vector>
#include "Match.hpp"
#include "ASTNode.hpp"


namespace parserlib
{


    /**
        A parse context implementation class which is based on an std container.
        @param InputType std container type; string by default.
     */
    template <
        typename InputType = std::string, 
        typename OutputType = std::vector<Match<InputType>>> 
    class ParseContext
    {
    public:
        ///iterator type.
        typedef typename InputType::const_iterator IteratorType;

        ///match type
        typedef typename OutputType::value_type MatchType;

        /**
            Constructor.
            @param input input.
         */
        ParseContext(InputType& input) :
            m_currentPosition(input.begin()),
            m_endPosition(input.end())
        {
        }

        /**
            Returns the current parsing position.
            @return the current parsing position.
         */
        IteratorType getCurrentPosition() const
        {
            return m_currentPosition;
        }

        /**
            Sets the current position.
            @param position current position.
         */
        void setCurrentPosition(IteratorType position)
        {
            m_currentPosition = position;
        }

        /**
            Returns the end position.
            @return the end position.
         */
        IteratorType getEndPosition() const
        {
            return m_endPosition;
        }

        /**
            Checks if the current position is valid.
            @return true if the current position is valid, false otherwise.
         */
        bool isValidPosition() const
        {
            return m_currentPosition < m_endPosition;
        }

        /**
            Checks if the current position is the end position.
            @return true if the current position is the end position, false otherwise.
         */
        bool isEndPosition() const
        {
            return m_currentPosition == m_endPosition;
        }

        /**
            Advances the current position.
         */
        void advance()
        {
            ++m_currentPosition;
        }

        /**
            Returns the current symbol.
            @return the current symbol.
         */
        auto getCurrentSymbol() const
        {
            return *m_currentPosition;
        }
        
        /**
            Returns the remaining input.
            @return the remaining input.
         */
        InputType getRemainingInput() const
        {
            return {m_currentPosition, m_endPosition};
        }

        /**
            Returns the current output.
            @return the current output.
         */
        const OutputType& getOutput() const
        {
            return m_output;
        }

        /**
            Returns the current output state.
            @return the current output state.
         */
        size_t getOutputState() const
        {
            return m_output.size();
        }

        /**
            Sets the output state.
            @param state output state.
         */
        void setOutputState(size_t state)
        {
            m_output.resize(state);
        }

        /**
            Adds a match.
            @param expression expression that was matched.
            @param start start of input.
            @param end end of input.
         */
        template <typename CreateASTFunctionType>
        void addMatch(
            const Expression* expression, 
            IteratorType start, 
            IteratorType end,
            CreateASTFunctionType&& createASTFunction)
        {
            m_output.emplace_back(expression, start, end, std::forward<CreateASTFunctionType>(createASTFunction));
        }

        /**
            Parses the current input.
            If the input is parsed successfully, then the AST is created.
            @param grammar grammar to use for parsing.
            @return a pointer to the created AST's root node, 
                or null if there was a parse error.
            @exception std::logic_error thrown if the AST tree is incomplete.
         */
        template <typename ResultType, typename ExpressionType> 
        std::shared_ptr<ResultType> parse(ExpressionType&& grammar)
        {
            //parse
            const bool parseOK = grammar.parse(*this);

            //if parsing failed or not the whole input was consumed, return error
            if (!parseOK || !isEndPosition()) return nullptr;

            //use this stack to create objects
            ASTNodeStack astNodeStack;

            //create the AST
            for (const MatchType& match : m_output)
            {
                match.getCreateASTFunction()(match, astNodeStack);
            }

            //check if there is only one object in the stack;
            //if not, then there is an issue
            if (astNodeStack.size() != 1)
            {
                throw std::logic_error("invalid AST configuration");
            }

            //return the root object
            return std::dynamic_pointer_cast<ResultType>(std::move(astNodeStack.front()));
        }

    private:
        //positions
        IteratorType m_currentPosition;
        IteratorType m_endPosition;

        //output 
        OutputType m_output;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSECONTEXT_HPP
