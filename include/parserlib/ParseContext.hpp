#ifndef PARSERLIB_PARSECONTEXT_HPP
#define PARSERLIB_PARSECONTEXT_HPP


#include <string>
#include <vector>
#include "Match.hpp"


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
            @param rule rule that was matched.
            @param start start of input.
            @param end end of input.
         */
        void addMatch(RuleExpression& rule, IteratorType start, IteratorType end)
        {
            m_output.emplace_back(rule, start, end);
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
