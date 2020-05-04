#ifndef PARSERLIB_TERMINALSEQUENCE_HPP
#define PARSERLIB_TERMINALSEQUENCE_HPP


#include <vector>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"


namespace parserlib
{


    /**
        A sequence of terminal symbols.
        @param SymbolType type of terminal symbol.
     */
    template <typename SymbolType = char> class TerminalSequence :
        public Expression,
        public UnaryOperatorsBase<TerminalSequence<SymbolType>>
    {
    public:
        /**
            Constructor.
            @param symbolArray array of symbols.
         */
        TerminalSequence(std::vector<SymbolType>&& symbolArray) :
            m_symbolArray(std::move(symbolArray))
        {
        }

        /**
            Constructor.
            @param null-terminated array of symbols.
         */
        TerminalSequence(const SymbolType* symbolArray) :
            m_symbolArray(symbolArray, endOf(symbolArray))
        {
        }

    private:
        //array of symbols.
        std::vector<SymbolType> m_symbolArray;

        //find end of symbol array
        static const SymbolType* endOf(const SymbolType* s)
        {
            for (; *s; ++s) {}
            return s;
        }
    };


    /**
        Specialization of expression type for type const char *.
     */
    template <> class ExpressionType<const char *>
    {
    public:
        ///expression type for const char *.
        typedef TerminalSequence<char> Type;
    };


    /**
        Specialization of expression type for type const wchar_t *.
     */
    template <> class ExpressionType<const wchar_t *>
    {
    public:
        ///expression type for const wchar_t *.
        typedef TerminalSequence<wchar_t> Type;
    };


    /**
        Specialization of expression type for type const char16_t *.
     */
    template <> class ExpressionType<const char16_t *>
    {
    public:
        ///expression type for const char16_t *.
        typedef TerminalSequence<char16_t> Type;
    };


    /**
        Specialization of expression type for type const char32_t *.
     */
    template <> class ExpressionType<const char32_t *>
    {
    public:
        ///expression type for const char32_t *.
        typedef TerminalSequence<char32_t> Type;
    };


    /**
        Helper function for creating terminal sequences.
        @param terminalSequence null-terminated array of symbols.
     */
    template <typename SymbolType>
    TerminalSequence<SymbolType> terminal(const SymbolType* terminalSequence)
    {
        return { terminalSequence };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALSEQUENCE_HPP
