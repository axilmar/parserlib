#ifndef PARSERLIB_TERMINALRANGEPARSER_HPP
#define PARSERLIB_TERMINALRANGEPARSER_HPP


#include "ParserNode.hpp"
#include "util.hpp"
#include "Error.hpp"


namespace parserlib {


    /**
     * A parser that parses a terminal against a range of values.
     * @param TerminalValueType value type of the terminal.
     */
    template <class TerminalValueType> class TerminalRangeParser 
        : public ParserNode<TerminalRangeParser<TerminalValueType>> {
    public:
        /**
         * Constructor.
         * @param minTerminalValue min terminal value.
         * @param maxTerminalValue max terminal value.
         */
        TerminalRangeParser(const TerminalValueType& minTerminalValue, const TerminalValueType& maxTerminalValue)
            : m_minTerminalValue(minTerminalValue), m_maxTerminalValue(maxTerminalValue) {
        }

        /**
         * Returns the min terminal value.
         * @return the min terminal value.
         */
        const TerminalValueType& minTerminalValue() const {
            return m_minTerminalValue;
        }

        /**
         * Returns the max terminal value.
         * @return the max terminal value.
         */
        const TerminalValueType& maxTerminalValue() const {
            return m_maxTerminalValue;
        }

        /**
         * Compares the current token, if there is one, against the range.
         * If equal, then it advances the source position by one.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (!pc.sourceEnded()) {
                if (pc.sourcePositionContains(m_minTerminalValue, m_maxTerminalValue)) {
                    pc.incrementSourcePosition();
                    return true;
                }
                else {
                    pc.addError(pc.sourcePosition(), [&]() {
                        return makeError(ErrorType::SyntaxError, pc.sourcePosition(),
                            toString("Syntax error: expected one of: ", tokenToString(m_minTerminalValue), "..", tokenToString(m_maxTerminalValue), ", found: ", *pc.sourcePosition().iterator()));
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
        const TerminalValueType m_minTerminalValue;
        const TerminalValueType m_maxTerminalValue;
    };


    /**
     * Helper function for creating a terminal range parser.
     * @param minTerminalValue min terminal value.
     * @param maxTerminalValue max terminal value.
     * @return a terminal range parser.
     */
    template <class TerminalValueType> 
    TerminalRangeParser<TerminalValueType> terminalRange(const TerminalValueType& minTerminalValue, const TerminalValueType& maxTerminalValue) {
        return { minTerminalValue, maxTerminalValue };
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINALRANGEPARSER_HPP
