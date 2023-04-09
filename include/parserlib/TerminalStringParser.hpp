#ifndef PARSERLIB_TERMINALSTRINGPARSER_HPP
#define PARSERLIB_TERMINALSTRINGPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser which parses a null-terminated string of terminal values.
     * @param TerminalValueType type of terminal value stored in the string.
     */
    template <class TerminalValueType> class TerminalStringParser
        : public ParserNode<TerminalStringParser<TerminalValueType>> {
    public:
        /**
         * Constructor.
         * @param string string.
         */
        TerminalStringParser(const TerminalValueType* string) : m_string(string) {
        }

        /**
         * Returns the string.
         * @return the string.
         */
        const TerminalValueType* string() const {
            return m_string;
        }
        
        /**
         * Parses the source against the string.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            auto sourcePos = pc.sourcePosition();
            auto stringPos = m_string;
            while (true) {
                if (!*stringPos) {
                    break;
                }
                if (sourcePos == pc.sourceEndPosition() || *sourcePos != *stringPos) {
                    return false;
                }
                ++sourcePos;
                ++stringPos;
            }
            pc.increaseSourcePosition(stringPos - m_string);
            return true;
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            return operator ()(pc);
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            return false;
        }

    private:
        const TerminalValueType* const m_string;
    };


    /**
     * Creates a terminal string parser out of a string.
     * @param string null-terminated string.
     * @return a terminal string parser.
     */
    template <class TerminalValueType>
    TerminalStringParser<TerminalValueType> terminal(const TerminalValueType* string) {
        return { string };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALSTRINGPARSER_HPP
