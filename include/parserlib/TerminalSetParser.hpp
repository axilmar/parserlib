#ifndef PARSERLIB_TERMINALSETPARSER_HPP
#define PARSERLIB_TERMINALSETPARSER_HPP


#include <vector>
#include <algorithm>
#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal out of a set of possible terminal values.
     * @param TerminalValueType value type of the terminal.
     */
    template <class TerminalValueType> class TerminalSetParser 
        : public ParserNode<TerminalSetParser<TerminalValueType>> {
    public:
        /**
         * Constructor.
         * @param terminalValues terminal values.
         */
        TerminalSetParser(const std::vector<TerminalValueType>& terminalValues)
            : m_terminalValues(terminalValues) {
            std::sort(m_terminalValues.begin(), m_terminalValues.end());
        }

        /**
         * Returns the terminal values.
         * @return the terminal values.
         */
        const std::vector<TerminalValueType>& terminalValues() const {
            return m_terminalValues;
        }

        /**
         * Checks if the current token is within the set of values.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (pc.sourcePosition() != pc.sourceEndPosition()) {
                const auto inputToken = *pc.sourcePosition();
                auto it = std::upper_bound(m_terminalValues.begin(), m_terminalValues.end(), inputToken);
                if (it != m_terminalValues.begin() && *std::prev(it) == inputToken) {
                    pc.incrementSourcePosition();
                    return true;
                }
            }
            return false;
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
        std::vector<TerminalValueType> m_terminalValues;
    };


    /**
     * Helper function for creating a terminal set parser.
     * @param terminalValue1 the 1st terminal value.
     * @param terminalValues the rest of terminal values.
     * @return a terminal parser.
     */
    template <class TerminalValueType, class ...T> 
    TerminalSetParser<TerminalValueType> 
    terminalSet(const TerminalValueType& terminalValue1, const T&... terminalValues) {
        return std::vector<TerminalValueType>{terminalValue1, terminalValues...};
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALSETPARSER_HPP
