#ifndef PARSERLIB_PARSECONTEXT_HPP
#define PARSERLIB_PARSECONTEXT_HPP


#include <string>


namespace parserlib
{


    /**
        A parse context implementation class which is based on an std container.
        @param InputType std container type; string by default.
     */
    template <typename InputType = std::string> class ParseContext
    {
    public:
        ///iterator type.
        typedef typename InputType::const_iterator IteratorType;

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

    private:
        //positions
        IteratorType m_currentPosition;
        IteratorType m_endPosition;

    };


} //namespace parserlib


#endif //PARSERLIB_PARSECONTEXT_HPP
