#ifndef PARSERLIB_SET_HPP
#define PARSERLIB_SET_HPP


#include <set>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


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
    Set<SymbolType> set(const std::initializer_list<SymbolType>& symbols)
    {
        return symbols;
    }


} //namespace parserlib


#endif //PARSERLIB_SET_HPP
