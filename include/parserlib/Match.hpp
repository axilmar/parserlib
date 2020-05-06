#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <string>
#include "RuleExpression.hpp"


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

        /**
            Constructor.
         */
        Match() :
            m_rule(nullptr)
        {
        }

        /**
            Constructor.
            @param rule rule that was matched.
            @param start input start.
            @param end input end.
         */
        Match(RuleExpression& rule, IteratorType start, IteratorType end) :
            m_rule(&rule),
            m_start(start),
            m_end(end)
        {
        }

        /**
            Returns the rule that was matched.
            @return the rule that was matched.
         */
        const RuleExpression& getRule() const
        {
            return *m_rule;
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

    private:
        RuleExpression* m_rule;
        IteratorType m_start;
        IteratorType m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
