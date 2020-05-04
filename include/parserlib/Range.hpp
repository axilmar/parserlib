#ifndef PARSERLIB_RANGE_HPP
#define PARSERLIB_RANGE_HPP

#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A range of symbols.
        @param SymbolType type of terminal symbol.
     */
    template <typename SymbolType = char> class Range :
        public Expression,
        public UnaryOperatorsBase<Range<SymbolType>>
    {
    public:
        /**
            Constructor.
            @param min min symbol.
            @param max max symbol.
         */
        Range(const SymbolType& min, const SymbolType& max) :
            m_min(min), m_max(max)
        {
        }

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            return false;
        }

    private:
        //min/max
        const SymbolType m_min;
        const SymbolType m_max;
    };


    /**
        Helper function for creating a range.
        @param min min symbol.
        @param max max symbol.
        @return a range expression.
     */
    template <typename SymbolType>
    Range<SymbolType> range(const SymbolType& min, const SymbolType& max)
    {
        return { min, max };
    }


} //namespace parserlib


#endif //PARSERLIB_RANGE_HPP
