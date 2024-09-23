#ifndef PARSERLIB_ANYTERMINALPARSER_HPP
#define PARSERLIB_ANYTERMINALPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that accepts any terminal.
     */
    class AnyTerminalParser : public ParserNode<AnyTerminalParser> {
    public:
        /**
         * Goes to the next terminal; returns true.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise (only if source is ended).
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            if (!pc.sourceEnded()) {
                pc.incrementSourcePosition();
                return true;
            }
            else {
                pc.addError(pc.sourcePosition(), [&]() {
                    return makeError(ErrorType::SyntaxError, pc.sourcePosition(), 
                        toString("Syntax error: expected symbol, found source end"));
                    });
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
    };


    /**
     * The one and only instance of AnyTerminalParser required.
     */
    inline const AnyTerminalParser any;


} //namespace parserlib


#endif //PARSERLIB_ANYTERMINALPARSER_HPP
