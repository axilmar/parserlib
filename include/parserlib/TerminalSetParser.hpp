#ifndef PARSERLIB_TERMINALSETPARSER_HPP
#define PARSERLIB_TERMINALSETPARSER_HPP


#include <vector>
#include "ParserNode.hpp"
#include "util.hpp"
#include "Error.hpp"


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
            : m_terminalValues(terminalValues)
        {
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
            if (!pc.sourceEnded()) {
                if (pc.sourcePositionContains(m_terminalValues)) {
                    pc.incrementSourcePosition();
                    return true;
                }
                else {
                    pc.addError(pc.sourcePosition(), [&]() {
                        return makeError(ErrorType::SyntaxError, pc.sourcePosition(),
                            toString("Syntax error: expected one of: ", m_terminalValues, ", found: ", *pc.sourcePosition().iterator()));
                        });
                }
            }
            return false;
        }

        /**
         * Does nothing; a terminal should not parse when a rule is expected to parse,
         * in order to continue after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return always false.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& /*pc*/, LeftRecursionContext<ParseContextType>& /*lrc*/) const {
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
