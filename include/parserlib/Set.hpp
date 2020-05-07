#ifndef PARSERLIB_SET_HPP
#define PARSERLIB_SET_HPP


#include <set>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "endOf.hpp"


namespace parserlib
{


    /**
        A set of symbols.
        @param SymbolType type of terminal symbol.
     */
    template <typename SymbolType = char> class Set :
        public Expression,
        public UnaryOperatorsBase<Set<SymbolType>>
    {
    public:
        /**
            Constructor.
            @param symbols symbols.
         */
        Set(const std::initializer_list<SymbolType>& symbols) :
            m_symbolSet(symbols.begin(), symbols.end())
        {
        }

        /**
            Constructor.
            @param symbols symbols.
         */
        Set(const SymbolType* symbols) :
            m_symbolSet(symbols, endOf(symbols))
        {
        }

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            if (pc.isValidPosition())
            {
                const auto symbol = pc.getCurrentSymbol();
                if (m_symbolSet.find(symbol) != m_symbolSet.end())
                {
                    pc.advance();
                    return true;
                }
            }
            return false;
        }

    private:
        //symbols
        std::set<SymbolType> m_symbolSet;
    };


    /**
        Helper function for creating a set.
        @param symbols.
        @return a set expression.
     */
    template <typename SymbolType>
    Set<SymbolType> oneOf(const std::initializer_list<SymbolType>& symbols)
    {
        return symbols;
    }


    /**
        Helper function for creating a set.
        @param symbols.
        @return a set expression.
     */
    template <typename SymbolType>
    Set<SymbolType> oneOf(const SymbolType* symbols)
    {
        return symbols;
    }


} //namespace parserlib


#endif //PARSERLIB_SET_HPP
