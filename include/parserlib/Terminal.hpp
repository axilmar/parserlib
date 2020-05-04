#ifndef PARSERLIB_TERMINAL_HPP
#define PARSERLIB_TERMINAL_HPP


#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A terminal.
        @param SymbolType type of terminal symbol.
     */
    template <class SymbolType = char> class Terminal :
        public Expression,
        public UnaryOperatorsBase<Terminal<SymbolType>>
    {
    public:
        /**
            Constructor.
            @param symbol symbol to parse.
         */
        Terminal(const SymbolType& symbol) :
            m_symbol(symbol)
        {
        }

    private:
        //symbol
        SymbolType m_symbol;
    };


    /**
        Specialization of expression type for type char.
        It yields a type Terminal<char>.
     */
    template <> class ExpressionType<char>
    {
    public:
        ///expression type for char.
        typedef Terminal<char> Type;
    };


    /**
        Specialization of expression type for type wchar_t.
        It yields a type Terminal<wchar_t>.
     */
    template <> class ExpressionType<wchar_t>
    {
    public:
        ///expression type for wchar_t.
        typedef Terminal<wchar_t> Type;
    };


    /**
        Specialization of expression type for type char16_t.
        It yields a type Terminal<char16_t>.
     */
    template <> class ExpressionType<char16_t>
    {
    public:
        ///expression type for char16_t.
        typedef Terminal<char16_t> Type;
    };


    /**
        Specialization of expression type for type char32_t.
        It yields a type Terminal<char32_t>.
     */
    template <> class ExpressionType<char32_t>
    {
    public:
        ///expression type for char32_t.
        typedef Terminal<char32_t> Type;
    };


    /**
        Helper function for creating terminals.
        @param symbol syhbol.
     */
    template <typename SymbolType>
    Terminal<SymbolType> terminal(const SymbolType& symbol)
    {
        return { symbol };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_HPP
