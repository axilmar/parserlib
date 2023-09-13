#ifndef PARSERLIB_EMPTYPARSER_HPP
#define PARSERLIB_EMPTYPARSER_HPP


#include "ParserNode.hpp"


namespace parserlib {


    /**
     * A parser that does not parse anything and always returns true.
     */
    class EmptyParser : public ParserNode<EmptyParser> {
    public:
        /**
         * Does nothing; returns true.
         * @param pc parse context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool operator ()(ParseContextType& /*pc*/) const {
            return true;
        }

        /**
         * Does nothing; returns true.
         * @param pc parse context.
         * @param lrc left recursion context.
         * @return true if parsing succeeds, false otherwise.
         */
        template <class ParseContextType> bool parseLeftRecursionContinuation(ParseContextType& /*pc*/, LeftRecursionContext<ParseContextType>& /*lrc*/) const {
            return true;
        }
    };


    /**
     * The one and only instance of empty parser required.
     */
    inline const EmptyParser _;


} //namespace parserlib


#endif //PARSERLIB_EMPTYPARSER_HPP
