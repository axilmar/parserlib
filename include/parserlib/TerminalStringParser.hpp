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
            //get the source position
            typename ParseContextType::PositionType sourcePos = pc.sourcePosition();

            //get the string position
            auto stringPos = m_string;

            while (true) {
                //if the string is exhausted, then the string was matched
                if (!*stringPos) {
                    break;
                }

                //if the source reached end, then the string could not be matched
                if (sourcePos == pc.sourceEnd()) {
                    return false;
                }

                //if the source position does not contain current character, 
                //then the string could not be matched
                if (!sourcePos.contains(*stringPos)) {
                    return false;
                }

                //next input/terminal character
                sourcePos.increment();
                ++stringPos;
            }

            //success; increment the source position by the appropriate number of places
            pc.increaseSourcePosition(stringPos - m_string);
            return true;
        }

        /**
         * Does nothing; a terminal should not parse when a rule is expected to parse,
         * in order to continue after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return always false.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
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
