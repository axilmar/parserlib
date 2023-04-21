#ifndef PARSERLIB_CWHITESPACEPARSER_HPP
#define PARSERLIB_CWHITESPACEPARSER_HPP


#include <cctype>
#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that parses whitespace using the function 'std::isspace'.
     */
    class CWhitespaceParser : public ParserNode<CWhitespaceParser> {
    public:
        /**
         * Parses whitespace.
         * @param pc parse context.
         * @return always true.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            while (!pc.sourceEnded()) {
                if (std::isspace(*pc.sourcePosition())) {
                    pc.incrementSourcePosition();
                }
                else {
                    break;
                }
            }
            return true;
        }

        /**
         * Whitespace is a terminal and therefore it should not work when parsing continuation.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return always false.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return false;
        }
    };
    

    /**
     * Creates a CWhitespaceParser instance.
     * @return a CWhitespaceParser instance.
     */
    inline CWhitespaceParser cwhitespace() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_CWHITESPACEPARSER_HPP
