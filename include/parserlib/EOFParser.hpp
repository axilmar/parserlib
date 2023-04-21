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
         * Checks if the source has ended.
         * @param pc parse context.
         * @return true if there is no more input to parse, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return pc.sourceEnded();
        }

        /**
         * Checks if the source has ended.
         * The object is called to parse within a left recursion parsing context,
         * in order to continue parsing after the non-left recursive part is parsed.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return operator ()(pc);
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
