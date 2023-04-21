#ifndef PARSERLIB_EMPTYPARSER_HPP
#define PARSERLIB_EMPTYPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that does nothing.
     */
    class EmptyParser : public ParserNode<EmptyParser> {
    public:
        /**
         * Does nothing.
         * @param pc parse context.
         * @return always true.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& pc) const {
            return true;
        }

        /**
         * Does nothing.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return always true.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& pc, LeftRecursionContext<ParseContextType>& lrc) const {
            return true;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_EMPTYPARSER_HPP
