#ifndef PARSERLIB_TERMINALSEQUENCE_HPP
#define PARSERLIB_TERMINALSEQUENCE_HPP


#include <vector>
#include "Expression.hpp"
#include "UnaryOperatorsBase.hpp"
#include "endOf.hpp"


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

        /**
            The parse function.
            @param pc parse context.
            @return true on success, false on failure.
         */
        template <typename ParseContextType> bool parse(ParseContextType& pc) const
        {
            const auto startPosition = pc.getCurrentPosition();

            for (auto it = m_symbolArray.begin();;)
            {
                //if the current sequence is exhausted, report success
                if (it == m_symbolArray.end())
                {
                    return true;
                }

                //if the input is exhausted, report a failure
                if (pc.isEndPosition())
                {
                    break;
                }

                //if the two symbols differ, report an error
                if (*it != pc.getCurrentSymbol())
                {
                    break;
                }

                //next symbols
                ++it;
                pc.advance();
            }

            //error
            pc.setCurrentPosition(startPosition);
            return false;
        }

    private:
        //array of symbols.
        std::vector<SymbolType> m_symbolArray;
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
