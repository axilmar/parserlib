#ifndef PARSERLIB_EOFPARSER_HPP
#define PARSERLIB_EOFPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that checks if there is no more input to parse. 
     */
    class EOFParser : public ParserNode<EOFParser> {
    public:
        /**
         * The parse function.
         * @param pc parse context.
         * @return true if there is no more input to parse, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return pc.sourceEnded();
        }

        /**
         * Parses a terminal under left recursion.
         * @param pc parse context.
         * @return true if a terminal was parsed, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionTerminal(ParseContextType& pc) const {
            return false;
        }

        /**
         * Parses a left recursion continuation.
         * @param pc parse context.
         * @return true on success, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc) const {
            return pc.sourceEnded();
        }
    };


    /**
     * Shortcut for creating an EOFParser.
     * @return an EOFParser instance.
     */
    inline EOFParser eof() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_EOFPARSER_HPP
